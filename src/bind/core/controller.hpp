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

#define STACK_RESERVE 65536

namespace bind { namespace core {

    inline controller::~controller(){ 
        if(!chains->empty()) printf("Bind:: exiting with operations still in queue!\n");
        this->clear();
        delete this->base_actor;
    }

    inline controller::controller() : chains(&stack_m), mirror(&stack_s), clock(1), sid(1) {
        this->stack_m.reserve(STACK_RESERVE);
        this->stack_s.reserve(STACK_RESERVE);
        this->garbage.reserve(STACK_RESERVE);

        this->base_actor = new actor_zero(this);
        actors.push(base_actor);
        this->push_scope(new bind::scope(get_num_procs()));

        if(!verbose()) this->io_guard.enable();
    }

    inline int controller::generate_sid(){
        return (++sid %= channel.tag_ub);
    }
    inline int controller::get_sid(){
        return sid;
    }
    inline void controller::deactivate(actor* a){
        actors.pop();
    }
    inline controller* controller::activate(actor* a){
        if(&get_actor() != this->base_actor) return NULL;
        actors.push(a);
        return this;
    }
    inline void controller::sync(){
        this->flush();
        this->clear();
        memory::cpu::instr_bulk::drop();
        memory::cpu::data_bulk::drop();
        memory::cpu::comm_bulk::drop();
    }
    inline actor& controller::get_actor(){
        return *actors.top();
    }
    inline scope& controller::get_scope(){
        return *scopes.top();
    }
    inline void controller::pop_scope(){
        scopes.pop();
    }
    inline void controller::push_scope(scope* s){
        scopes.push(s);
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

    inline void controller::sync(revision* r){
        if(is_serial()) return;
        if(model::common(r)) return;
        if(model::local(r)) set<revision>::spawn(*r);
        else get<revision>::spawn(*r);
    }

    inline void controller::lsync(revision* r){
        if(model::common(r)) return;
        if(!model::local(r)) get<revision>::spawn(*r);
    }

    inline void controller::rsync(revision* r){
        if(model::common(r)) return;
        if(model::local(r)) set<revision>::spawn(*r);
        else get<revision>::spawn(*r); // assist
    }

    inline void controller::lsync(transformable* v){
        if(is_serial()) return;
        set<transformable>::spawn(*v);
    }

    inline void controller::rsync(transformable* v){
        get<transformable>::spawn(*v);
    }

    template<typename T> void controller::collect(T* o){
        this->garbage.push_back(o);
    }

    inline void controller::squeeze(revision* r) const {
        if(r->valid() && !r->referenced() && r->locked_once()){
            if(r->spec.signature == memory::cpu::standard::signature){
                r->spec.free(r->data);
                r->spec.signature = memory::delegated::signature;
            }
        }
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

    inline rank_t controller::get_shared_rank() const {
        return get_num_procs();
    }

    inline bool controller::is_serial() const {
        return (get_num_procs() == 1);
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

#undef STACK_RESERVE
