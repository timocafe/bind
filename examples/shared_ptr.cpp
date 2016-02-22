#include "utils/bind.hpp"
#define ARRAY_LENGTH 256

// FNV-1a 64-bit non-cryptographic hashing
uint64_t fnv_64a_buf(const void* buf, size_t len){
    unsigned char* bp = (unsigned char*)buf;
    unsigned char* be = bp + len;
    uint64_t hval = ((uint64_t)0xcbf29ce484222325ULL);

    while(bp < be){
        hval ^= (uint64_t)*bp++;
        hval *= ((uint64_t)0x100000001b3ULL);
    }
    return hval;
}

int main(){
    std::vector<bind::array<char> > phrases;
    std::vector<bind::shared_ptr<uint64_t> > hashes(bind::nodes::size(), 0);

    // Generating test input array on each node
    std::for_each(bind::nodes::begin(), bind::nodes::end(), [&](int i){
        bind::node n(i);
        phrases.push_back(bind::array<char>(ARRAY_LENGTH));
        // Storing 'a' or 'b' depending upon the node number
        phrases.back().fill('a'+(i % 2));
    });

    // Calculating test input hashes on each node
    std::for_each(bind::nodes::begin(), bind::nodes::end(), [&](int i){
        bind::node(i).cpu([](const bind::array<char>& input, bind::shared_ptr<uint64_t>& hash){
            *hash = fnv_64a_buf(input.data(), input.size()*sizeof(char));
        }, phrases[i], hashes[i]);
    });

    // Waiting for operations to complete.
    // All hashes are going to be broadcasted due to the shared_ptr.
    bind::sync();

    // Removing duplicates and printing
    std::set<uint64_t> set;
    for(auto& hash : hashes) set.insert( *hash );
    for(auto hash : set) std::cout << hash << "\n";

    return 0;
}
