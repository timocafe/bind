#include "utils/bind.hpp"
#include "utils/timer.hpp"
#include <cassert>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <bitset>
#include <iostream>
#include <stdexcept>
#include <nmmintrin.h>
#include <memory.h>
#include <bitset>

#define uint uint64_t
#define BITS 64
#define M 1024
#define M_t (M/BITS)
#define SPREAD (256*256)
#define NSTREAMS 10

// {{{ utils
namespace detail {
    template<int SR> void shift_right(uint& v){ v = v >> SR; }
    template<> void shift_right<BITS>(uint& v){ v = 0; }
    template<> void shift_right<0>(uint& v){}

    template<int SL> void shift_left(uint& v){ v = v << SL; }
    template<> void shift_left<BITS>(uint& v){ v = 0; }
    template<> void shift_left<0>(uint& v){}

    void shift_array(uint* x, int n, int times = 1){
        while(times--){
            int carry = 0;
            for(int i = 0; i < n/BITS; i++){
                int next = (x[i] & 1) ? 1 : 0;
                x[i] = x[i] >> 1;
                x[i] |= (uint)carry << (BITS-1);
                carry = next;
            }
        }
    }

    void init_random(uint* x, int n){
        for(int i = 0; i < n/(sizeof(unsigned int)*8); ++i) ((unsigned int*)x)[i] = rand();
    }

    void print(uint* x, int n){
        for(int i = 0; i < n/BITS; i++){
            for(int b = BITS-1; b >= 0; b--){
                uint p = x[i];
                if(b) p = p >> b;
                shift_left<BITS-1>(p);
                shift_right<BITS-1>(p);
                std::cout << p;
            }
        }
    }
}
// }}}

void filter_cpu(uint* y, const uint* R, const uint* x){
    uint head_mask[2] __attribute__ ((aligned (16)));
    uint tail_mask[2] __attribute__ ((aligned (16)));
    head_mask[1] = tail_mask[0] = 0xFFFFFFFFFFFFFFFF;

    for(int s = 0; s < SPREAD*M_t; s++){
        tail_mask[1] = 0;
        for(unsigned bit = 0; bit < BITS; ++bit){
            head_mask[0] = 0xFFFFFFFFFFFFFFFF >> bit;

            __m128i mm_parity = _mm_xor_si128( _mm_and_si128( _mm_loadu_si128((__m128i *)x), _mm_loadu_si128((__m128i *)head_mask) ),
                                               _mm_loadu_si128((__m128i *)(R + bit*M_t)) );
            for(unsigned i = 2; i < (M_t-2); i += 2)
                mm_parity = _mm_or_si128( mm_parity , _mm_xor_si128(_mm_loadu_si128((__m128i *)(x + i)), _mm_loadu_si128((__m128i *)(R + i + bit*M_t))) );

            mm_parity = _mm_or_si128( mm_parity, _mm_xor_si128( _mm_and_si128( _mm_loadu_si128((__m128i *)(x + M_t-2)), _mm_loadu_si128((__m128i *)tail_mask) ),
                                                                _mm_loadu_si128((__m128i *)(R + M_t-2 + bit*M_t))
                                                               ));
            if(_mm_testz_si128(mm_parity, mm_parity)){
                printf("FOUND %d at %d\n", bit, s);
                *y |= ((uint)1 << bit);
            }
            tail_mask[1] = 0xFFFFFFFFFFFFFFFF << (63-bit);
        }
        x++; y++;
    }
}

int main(){
    assert(M % 128 == 0); srand(42);

    std::vector<bind::array<uint> > y_streams; y_streams.reserve(NSTREAMS);
    std::vector<bind::array<uint> > x_streams; x_streams.reserve(NSTREAMS); // SPREAD*16 uints each
    bind::array<uint> r(BITS * M_t);

    for(int k = 0; k < NSTREAMS; k++){
        y_streams.push_back(bind::array<uint>(SPREAD*M/8 / sizeof(uint)));
        x_streams.push_back(bind::array<uint>(SPREAD*M/8 / sizeof(uint)));
    }

    // Filling template R
    bind::cpu([](volatile bind::array<uint>& r){
        uint* r_ = r.data();
        detail::init_random(r_, M); detail::shift_right<0>(r_[0]); detail::shift_left<BITS>(r_[M_t-1]);
        for(int b = 1; b < BITS; b++) memcpy(&r_[b*M_t], r_, M/8);
        for(int b = 1; b < BITS; b++) detail::shift_array(&r_[b*M_t], M, b);
    }, r);

    // Filling input X with random bits
    for(int k = 0; k < NSTREAMS; ++k)
    bind::cpu([](volatile bind::array<uint>& x){
        detail::init_random(x.data(), M*SPREAD);
    }, x_streams[k]);

    // Hidding our pattern somewhere
    bind::cpu([](bind::array<uint>& x, const bind::array<uint>& r){
        memcpy(&x[SPREAD*9], &r[31*M_t], M/8); // i.e. SPREAD *9
    }, x_streams[0], r);

    // Executing
    bind::timer t_bind("Filtering time"); t_bind.begin();

    for(int k = 0; k < NSTREAMS; ++k){
        bind::cpu([](bind::array<uint>& y, const bind::array<uint>& r, const bind::array<uint>& x){
            filter_cpu(y.data(), r.data(), x.data());
        }, y_streams[k], r, x_streams[k]);
    }

    bind::sync();
    t_bind.end();
    return 0;
}

