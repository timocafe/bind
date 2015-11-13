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

#ifndef BIND_CORE_CONTROLLER_H
#define BIND_CORE_CONTROLLER_H

namespace bind { namespace core {

    using model::history;
    using model::revision;
    using model::any;
    using model::functor;

    class controller {
        controller(const controller&) = delete;
        controller& operator=(const controller&) = delete;
        controller(); 
    public:
        typedef transport::BIND_CHANNEL_NAME::channel channel_type;
       ~controller();
        void flush();
        void clear();
        bool queue (functor* f);
        bool update(revision& r);

        template<class Device, locality L, typename T> void sync(T* o);
        template<typename T> void collect(T* o);
        void squeeze(revision* r) const;

        void touch(const history* o, rank_t owner);
        void use_revision(history* o);
        template<locality L>
        void add_revision(history* o, functor* g, rank_t owner);

        bool verbose() const;
        rank_t get_rank() const;
        int get_num_procs() const;
        channel_type& get_channel();

        void sync();

        controller* activate(node* a);
        void deactivate(node* a);
        node& get_node();
    private:
        size_t clock;
        channel_type channel;
        std::vector< functor* > stack_m;
        std::vector< functor* > stack_s;
        std::vector< functor* >* chains;
        std::vector< functor* >* mirror;
        memory::collector garbage;
        utils::funneled_io io_guard;
        node_each* each;
        node* which;
    public:
        std::vector<rank_t> nodes;
        template<class T>
        struct weak_instance {
            static controller w;
        };
    };
    
} }

namespace bind {
    template<class T> core::controller core::controller::weak_instance<T>::w;
    inline core::controller& select(){ return core::controller::weak_instance<void>::w; }
}

#endif
