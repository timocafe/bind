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

#ifndef BIND_TRANSPORT_MPI_CHANNEL_HPP
#define BIND_TRANSPORT_MPI_CHANNEL_HPP

namespace bind { namespace transport { namespace mpi {

    inline void recv_impl(request_impl* r){
        MPI_Irecv(r->data, r->extent, MPI_DOUBLE, r->target, r->tag, MPI_COMM_WORLD, &r->mpi_request);
    }
    inline void send_impl(request_impl* r){
        MPI_Isend(r->data, r->extent, MPI_DOUBLE, r->target, r->tag, MPI_COMM_WORLD, &r->mpi_request);
    }
    inline bool test_impl(request_impl* r){
        int f = 0; MPI_Test(&r->mpi_request, &f, MPI_STATUS_IGNORE); return f;
    }

    inline channel::mount::mount(){
        int *ub, flag, level, zero = 0;
        MPI_Init_thread(&zero, NULL, MPI_THREAD_FUNNELED, &level); 
        if(level != MPI_THREAD_FUNNELED) throw std::runtime_error("Error: Wrong threading level");
        MPI_Comm_size(MPI_COMM_WORLD, &np);
        MPI_Comm_rank(MPI_COMM_WORLD, &self);
        MPI_Attr_get(MPI_COMM_WORLD, MPI_TAG_UB, &ub, &flag);
        this->tag_ub = flag ? *ub : 32767;
        this->sid = 1;
        
        trees.resize(2); // 0,1 are empty
        for(int i = 2; i <= np; i++)  trees.push_back(new binary_tree<rank_t>(i));
        for(int i = 0; i < 2*np; i++) circle.push_back(i % np);
    }

    inline channel::mount::~mount(){
        for(binary_tree<rank_t>* t : trees) delete t;
        MPI_Finalize();
    }
    
    inline channel::~channel(){
        delete this->world;
    }

    inline channel::channel(){
        channel::setup(); // making sure MPI is initialised
        this->world = new group(MPI_COMM_WORLD);
        this->rank = this->world->rank;
    }

    inline void channel::barrier(){
        MPI_Barrier(MPI_COMM_WORLD);
    }

    inline size_t channel::dim() const {
        return this->world->size;
    }

    inline collective<typename channel::scalar_type>* channel::bcast(scalar_type& v, rank_t root){
        return new collective<scalar_type>(v, root);
    }

    inline collective<typename channel::scalar_type>* channel::bcast(scalar_type& v){
        return new collective<scalar_type>(v, rank);
    }

    inline collective<typename channel::block_type>* channel::get(block_type& r){
        return new collective<block_type>(r, r.owner);
    }

    inline collective<typename channel::block_type>* channel::set(block_type& r){
        return new collective<block_type>(r, rank);
    }

} } }

#endif
