#include "utils/bind.hpp"
#include <unordered_map>
#include <mutex>

// {{{ helper traits
namespace std {
    template<typename T>
    struct hash<std::pair<T,int> > : public hash<int> {
        std::size_t operator()(const std::pair<T,int>& k) const {
            return hash<int>::operator()(k.first);
        }
    };
}

namespace bind {
    template<typename T>
    void local_read_in(bind::array<T>& array, std::vector<T>* local_ptr){
        for(int k = 0; k < local_ptr->size(); k++) array[k] = (*local_ptr)[k];
    }

    template<typename T>
    bind::array<T> make_array(std::vector<T>& local_vector, int global_size, int rank){
        bind::array<T> result(global_size);
        bind::node(rank).cpu(local_read_in<T>, result, &local_vector);
        return result;
    }
}

template<class> struct vector_type;

template<class First, class Second>
struct vector_type<std::vector<std::pair<First, Second> > > {
    typedef First key_type;
    typedef Second value_type;
};

template <typename Function>
struct function_traits: public function_traits<decltype(&Function::operator())> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> {
    typedef ReturnType (*pointer)(Args...);
    typedef const std::function<ReturnType(Args...)> function;

    typedef typename vector_type< ReturnType >::key_type key_type;
    typedef typename vector_type< ReturnType >::value_type value_type;
};
// }}}

template<typename Key, typename Value>
class KVPairs {
public:
    using key_type = Key;
    using value_type = Value;
    using kvp_type = std::pair<key_type, value_type>;
    using size_type = int;
    using rank_type = int;
    template<typename F> using OtherKVPairs = KVPairs<typename function_traits<F>::key_type, typename function_traits<F>::value_type>;
    using container_type = std::unordered_map<std::pair<Key, rank_type>, std::vector<Value> >;

    operator std::unordered_map<Key, std::vector<Value> > (){
        std::unordered_map<Key, std::vector<Value> > res;
        for(auto& p : map_) res[p.first.first] = p.second;
        return res;
    }
    KVPairs(std::unordered_map<Key, std::vector<Value> >& map){
        for(auto& p : map) map_[{ p.first, bind::rank() }] = p.second;
    }

    KVPairs(container_type& map) : map_(map) { }

    template<typename F>
    auto map(F map_fn) -> OtherKVPairs<F> {
        typename OtherKVPairs<F>::container_type other_map;
        std::vector< std::vector< typename OtherKVPairs<F>::kvp_type > > kv_pairs_vector(map_.size()); int v = 0;

        for(auto& pairs : map_) if(pairs.first.second == bind::rank())
            kv_pairs_vector[v++] = cilk_spawn map_fn(pairs.first.first, pairs.second);
        cilk_sync;
        for(auto& kv_pairs : kv_pairs_vector){
            for(auto& p : kv_pairs) other_map[{ p.first, bind::rank() }].push_back( p.second ); // group by key
        }
        return other_map;
    }

    template<typename F>
    auto combine(F combine_fn) -> OtherKVPairs<F> {
        return map(combine_fn);
    }

    template<typename F>
    auto reduce(F reduce_fn) -> OtherKVPairs<F> {
        this->shuffle();
        return map(reduce_fn);
    }

    // {{{ shuffle
private:
    void shuffle(){
        // Calculating local keys counts
        std::vector<std::pair<key_type, size_type> > local_k_counts;
        for(auto p : map_) local_k_counts.push_back({ p.first.first, p.second.size() });
        
        // Gathering local_k_counts sizes across nodes
        std::vector<bind::shared_ptr<size_type> > k_counts_sizes(bind::num_procs(), 0);
        std::for_each(bind::nodes::begin(), bind::nodes::end(), [&](rank_type i){
            bind::node(i).cpu([](bind::shared_ptr<size_type>& size, size_t local_size){ *size = local_size; }, k_counts_sizes[i], local_k_counts.size());
        }); bind::sync();
        
        // Creating global all_k_counts using the sizes
        std::vector< bind::array<std::pair<key_type, size_type> > > all_k_counts; all_k_counts.reserve(bind::num_procs());
        std::for_each(bind::nodes::begin(), bind::nodes::end(), [&](rank_type i){
            all_k_counts.push_back( bind::make_array(local_k_counts, *k_counts_sizes[i], i) );
        });
        // Node #0                  Node #1                 Node #p
        // K1  K2   ...   Kn        K1  K2   ...   Kn       K1  K2   ...   Kn
        // c1  c2         cn        c1  c2         cn       c1  c2         cn
        
        // Reducing global all_k_counts into <key>:<ranks/counts>-map (can be done in log)
        std::unordered_map<key_type, 
                           std::pair<size_type, std::vector< std::pair<rank_type, size_type> > >
                          > kcs_map;

        bind::array<int> order_guard(1); // enforcing deterministic order of kcs_map
        std::for_each(bind::nodes::begin(), bind::nodes::end(), [&, this](rank_type i){
            bind::cpu([](bind::array<int>&, const bind::array<std::pair<key_type, size_type> >& counts, rank_type r, decltype(kcs_map)* map){
                for(auto count : counts){
                    auto& row = (*map)[count.first];
                    row.second.push_back({ r, count.second });
                    row.first += count.second; // total size
                }
            }, order_guard, all_k_counts[i], i, &kcs_map);
        });
        bind::sync();
        
        // Getting all of the values
        std::vector< std::pair< std::pair<Key, rank_type>, std::vector<Value> > > global_values_lists; global_values_lists.reserve(kcs_map.size());
        rank_type r = 0;
        for(auto& row : kcs_map){
            auto key = row.first;
            global_values_lists.push_back({ {key, r}, {} });
        
            for(auto& part : row.second.second){
                bind::array<value_type> local_values = bind::make_array(map_[{ key, part.first }], part.second, part.first);
        
                bind::node(r).cpu([](std::vector<value_type>* global, const bind::array<value_type>& local){
                    static std::mutex m; std::lock_guard<std::mutex> lock(m);
                    for(auto value : local) global->push_back(value); // append this part to node #r
                }, &global_values_lists.back().second, local_values);
            }
            ++r %= bind::num_procs(); // round-robin
        }
        bind::sync();

        container_type shuffled_map;
        for(auto& row : global_values_lists) shuffled_map.insert(row);
        std::swap(shuffled_map, map_);
    }
    // }}}
private:
    container_type map_;
};


int main(){
    std::vector< std::string > strings = { "Thus the KVPairs framework transforms a list of (key, value) pairs into a list of values.",
                                           "This behavior is different from the typical functional programming map and reduce combination,",
                                           "which accepts a list of arbitrary values and returns one single value",
                                           "that combines all the values returned by map." };

    for(int i = 0; i < std::min(strings.size(), (size_t)bind::num_procs()); i++)
        std::cout << strings[i] << "\n";

    // Initializing node's local input key/values map.
    std::unordered_map<int, std::vector<std::string> > local_map = { //{ <key>,        { <values> }}
                                                                       { bind::rank(), { strings[bind::rank()] }},
                                                                   };
    // Counting the letters in the global test input.
    // Reduce step contains implicit "shuffle" that transfers the data between the nodes.

    std::unordered_map<char, std::vector<int> > res =
        KVPairs<int, std::string>(local_map)
        .map([](int key, std::vector<std::string>& values) -> std::vector< std::pair<char,int> > {
            std::vector< std::pair<char, int> > kv_pairs;
            for(auto str : values) for(auto e : str)
            kv_pairs.push_back({ e, 1 });
            return kv_pairs;
        })
        .combine([](char key, std::vector<int>& values) -> std::vector< std::pair<char,int> > {
            return { { key, std::accumulate(values.begin(), values.end(), 0) } };
        })
        .reduce([](char key, std::vector<int>& values) -> std::vector< std::pair<char,int> > {
            return { { key, std::accumulate(values.begin(), values.end(), 0) } };
        });

    // Printing local results
    for(auto e : res) std::cerr << "Reduce key <" << e.first << ">: " << e.second[0] << "\n";
    return 0;
}
