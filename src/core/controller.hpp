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

#ifndef BIND_CORE_CONTROLLER_HPP
#define BIND_CORE_CONTROLLER_HPP

namespace bind { namespace nodes {
    inline size_t size(){
        return select().nodes.size();
    }
    inline std::vector<rank_t>::const_iterator begin(){
        return select().nodes.begin();
    }
    inline std::vector<rank_t>::const_iterator end(){
        return select().nodes.end();
    }
    inline rank_t which_(){
        return select().get_node().which();
    }
    template<typename V>
    inline rank_t which(const V& o){
        return o.allocator_.desc->current->owner;
    }
    inline rank_t which(){
        rank_t w = which_();
        return (w == select().get_num_procs() ? select().get_rank() : w);
    }
} }

namespace bind { namespace transport {

    using model::revision;
    using model::any;

    template<class Device, locality L = locality::common>
    struct hub {
        static void sync(revision* r){
            if(bind::nodes::size() == 1) return; // serial
            if(model::common(r)) return;
            if(model::local(r)) core::set<revision>::spawn(*r);
            else core::get<revision>::spawn(*r);
        }
    };

    template<class Device>
    struct hub<Device, locality::local> {
        static void sync(revision* r){
            if(model::common(r)) return;
            if(!model::local(r)) core::get<revision>::spawn(*r);
        }
        static void sync(any* v){
            if(bind::nodes::size() == 1) return;
            core::set<any>::spawn(*v);
        }
    };

    template<class Device>
    struct hub<Device, locality::remote> {
        static void sync(revision* r){
            if(r->owner == bind::nodes::which_() || model::common(r)) return;
            if(model::local(r)) core::set<revision>::spawn(*r);
            else core::get<revision>::spawn(*r); // assist
        }
        static void sync(any* v){
            core::get<any>::spawn(*v);
        }
    };

} }

namespace bind { namespace core {

    inline controller::~controller(){ 
        if(!chains->empty()) printf("Bind:: exiting with operations still in queue!\n");
        this->clear();
        delete this->each;
    }

    inline controller::controller() : chains(&stack_m), mirror(&stack_s), clock(1) {
        this->each = new node_each(this);
        this->which = NULL;
        for(int i = 0; i < get_num_procs(); i++) nodes.push_back(i);
        if(!verbose()) this->io_guard.enable();
    }

    inline void controller::deactivate(node* a){
        which = NULL;
    }

    inline controller* controller::activate(node* n){
        if(which) return NULL;
        which = n;
        return this;
    }

    inline void controller::sync(){
        this->flush();
        this->clear();
        memory::cpu::instr_bulk::drop();
        memory::cpu::data_bulk::drop();
        memory::cpu::comm_bulk::drop();
    }

    inline node& controller::get_node(){
        return (!which) ? *each : *which;
    }

    inline void controller::flush(){
        while(!chains->empty()){
            for(auto task : *chains){
                if(task->ready()){
                    cilk_spawn task->invoke();
                    for(auto d : task->deps) d->ready();
                    mirror->insert(mirror->end(), task->deps.begin(), task->deps.end());
                }else mirror->push_back(task);
            }
            chains->clear();
            std::swap(chains,mirror);
        }
        cilk_sync;
        clock++;
        channel.barrier();
    }

    inline void controller::clear(){
        this->garbage.clear();
    }

    inline bool controller::queue(functor* f){
        this->chains->push_back(f);
        return true;
    }

    inline bool controller::update(revision& r){
        if(r.assist.first != clock){
            r.assist.first = clock;
            return true;
        }
        return false;
    }

    template<class Device, locality L, typename T>
    inline void controller::sync(T* o){
        transport::hub<Device, L>::sync(o);
    }

    template<typename T> void controller::collect(T* o){
        this->garbage.push_back(o);
    }

    inline void controller::squeeze(revision* r) const {
        this->garbage.squeeze(r);
    }

    inline void controller::touch(const history* o, rank_t owner){
        if(o->back() == NULL)
            const_cast<history*>(o)->init_state(owner);
    }

    inline void controller::use_revision(history* o){
        o->back()->use();
    }

    template<locality L>
    void controller::add_revision(history* o, functor* g, rank_t owner){
        o->add_state<L>(g, owner);
    }

    inline rank_t controller::get_rank() const {
        return channel.rank;
    }

    inline bool controller::verbose() const {
        return (get_rank() == 0);
    }

    inline int controller::get_num_procs() const {
        return channel.dim();
    }

    inline typename controller::channel_type & controller::get_channel(){
        return channel;
    }

} }

#endif
