#include "utils/bind.hpp"
#include <unordered_map>

namespace detail {
    template<typename T>
    void local_read_in(bind::array<T>& array, std::vector<T>* local_ptr){
        for(int k = 0; k < local_ptr->size(); k++) array[k] = (*local_ptr)[k];
    }
}

template<typename K2, typename V2>
class MapReduce {
    using key_type = K2;
    using value_type = V2;
    using size_type = int;
    using rank_type = int;
public:

    template<typename K1, typename V1, typename Fn>
    MapReduce& map(Fn map_fn, K1 input_key, V1 input_value){
        std::vector< std::pair<key_type,value_type> > kv_pairs = map_fn(input_key, input_value);
        for(auto p : kv_pairs) kvs_map[p.first].push_back( p.second ); // group by key
        return this->shuffle();
    }

    template<typename Fn>
    MapReduce& reduce(Fn reduce_fn){
        bind::array<int> order(1); // enforcing deterministic order
        int r = 0;
        for(auto& row : value_lists){
            bind::node(r).cpu([reduce_fn](key_type key, std::vector<value_type>* values, bind::array<int>&){
                reduce_fn(key, *values);
            }, row.first, &row.second, order);
            ++r %= bind::num_procs();
        }
        bind::sync();
        return *this;
    }

    MapReduce& shuffle(){
        // Calculating local keys counts
        std::vector<std::pair<key_type, size_type> > local_k_counts;
        for(auto p : kvs_map) local_k_counts.push_back({ p.first, p.second.size() });
        
        // Gathering local_k_counts sizes across nodes
        std::vector<bind::shared_ptr<size_type> > k_counts_sizes(bind::num_procs(), 0);
        std::for_each(bind::nodes::begin(), bind::nodes::end(), [&](rank_type i){
            bind::node(i).cpu([](bind::shared_ptr<size_type>& size, size_t local_size){ *size = local_size; }, k_counts_sizes[i], local_k_counts.size());
        }); bind::sync();
        
        // Creating global all_k_counts using the sizes
        std::vector< bind::array<std::pair<key_type, size_type> > > all_k_counts; all_k_counts.reserve(bind::num_procs());
        for(auto size : k_counts_sizes) all_k_counts.push_back( bind::array<std::pair<key_type, rank_type> >(*size) );
        
        // Filling in the global all_k_counts
        std::for_each(bind::nodes::begin(), bind::nodes::end(), [&](rank_type i){
            bind::node(i).cpu(detail::local_read_in<std::pair<key_type, size_type> >, all_k_counts[i], &local_k_counts);
        });
        
        // Node #0                  Node #1                 Node #p
        // K1  K2   ...   Kn        K1  K2   ...   Kn       K1  K2   ...   Kn
        // c1  c2         cn        c1  c2         cn       c1  c2         cn
        
        // Reducing global all_k_counts into <key>:<ranks/counts>-map (can be done in log)
        bind::array<int> order(1); // enforcing deterministic order of kcs_map
        std::for_each(bind::nodes::begin(), bind::nodes::end(), [&, this](rank_type i){
            bind::cpu([](const bind::array<std::pair<key_type, size_type> >& counts, rank_type r, decltype(kcs_map)* map, bind::array<int>& ){
                for(int k = 0; k < counts.size(); k++){
                    auto& row = (*map)[counts[k].first];
                    row.second.push_back({ r, counts[k].second });
                    row.first += counts[k].second; // total size
                }
            }, all_k_counts[i], i, &kcs_map, order);
        });
        bind::sync();
        
        // Getting all of the values
        rank_type r = 0; value_lists.reserve(kcs_map.size());
        for(auto& row : kcs_map){
            auto key = row.first;
            value_lists.push_back({ key, {} }); value_lists.back().second.reserve(row.second.first);
        
            bind::array<int> serial_order(1);
            for(auto& part : row.second.second){
                bind::array<value_type> local_values(part.second);
                bind::node(part.first).cpu(detail::local_read_in<value_type>, local_values, &kvs_map[key]);
        
                bind::node(r).cpu([](std::vector<value_type>* all, const bind::array<value_type>& local, bind::array<int>&){
                    for(int i = 0; i < local.size(); i++) all->push_back( local[i] ); // append this part to node #r
                }, &value_lists.back().second, local_values, serial_order);
            }
            ++r %= bind::num_procs();
        }

        bind::sync();
        return *this;
    }

    void print_kcs_map(){
        for(auto& row : kcs_map){
            std::cout << "Key <" << row.first << "> (total " << row.second.first << "): ";
            for(auto& part : row.second.second) std::cout << "r" << part.first << "/" << part.second << " ";
            std::cout << "\n";
        }
    }

private:
    std::unordered_map<key_type,
                       std::vector<value_type>
                      > kvs_map;

    std::unordered_map<key_type, 
                       std::pair<size_type, std::vector< std::pair<rank_type, size_type> > >
                      > kcs_map;

    std::vector< std::pair<key_type, std::vector<value_type> > > value_lists;
};

int main(){
    std::vector< std::string > strings = { "Thus the MapReduce framework transforms a list of (key, value) pairs into a list of values.",
                                           "This behavior is different from the typical functional programming map and reduce combination,",
                                           "which accepts a list of arbitrary values and returns one single value",
                                           "that combines all the values returned by map." };

    for(int i = 0; i < bind::num_procs(); i++)
        std::cout << strings[i] << "\n";

    MapReduce<char, int>().map([](int key, std::string& value) -> std::vector< std::pair<char,int> > {
                               std::vector< std::pair<char, int> > kv_pairs;
                               for(auto e : value) kv_pairs.push_back({ e, 1 });
                               return kv_pairs;
                           }, bind::rank(), strings[bind::rank()])
                          .reduce([](char key, std::vector<int>& values){
                               std::cerr << "Reduce key <" << key << ">: ";
                               std::cerr << std::accumulate(values.begin(), values.end(), 0);
                               std::cerr << "\n";
                           });
    return 0;
}
