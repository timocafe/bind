/*
 * Copyright Institute for Theoretical Physics, ETH Zurich 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef BIND_TRANSPORT_MPI_CHANNEL
#define BIND_TRANSPORT_MPI_CHANNEL

namespace bind { namespace transport { namespace mpi {

    class request_impl;
    template<class T> class collective;

    static void recv_impl(request_impl* r);
    static void send_impl(request_impl* r);
    static bool test_impl(request_impl* r);

    class channel {
    public:
        typedef typename model::revision block_type;
        typedef typename model::transformable scalar_type;
        template<class T> using collective_type = collective<T>;
        struct mount {
            mount(); 
           ~mount();
            std::vector<binary_tree<rank_t>*> trees;
            std::vector<rank_t> circle;
            int tag_ub;
            int sid;
            int self;
            int np;
        };
        static mount& setup(){ 
            static mount m; 
            return m; 
        }
        channel();
        size_t dim() const;
        static void barrier();
        collective<block_type>* get(block_type& r);
        collective<block_type>* set(block_type& r);
        collective<scalar_type>* bcast(scalar_type& v, rank_t root);
        collective<scalar_type>* bcast(scalar_type& v);
        rank_t rank;
        group* world;
    };

} } }

#endif
