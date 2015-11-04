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

#ifndef BIND_INTERFACE_KERNEL
#define BIND_INTERFACE_KERNEL

namespace bind {

    using model::functor;

    template<class K>
    class kernel : public functor {
    public:
        #define inliner kernel_inliner<typename K::ftype,K::c>
        inline void operator delete (void* ptr){ }
        inline void* operator new (size_t size){
            return memory::cpu::instr_bulk::malloc<sizeof(K)+sizeof(void*)*inliner::arity>();
        }
        virtual bool ready() override { 
            return inliner::ready(this);
        }
        virtual void invoke() override {
            inliner::invoke(this);
            inliner::cleanup(this);
        }
        template<size_t...I, typename... Args>
        static void expand_spawn(index_sequence<I...>, Args&... args){
            inliner::latch(new kernel(), args...);
        }
        template<typename... Args>
        static inline void spawn(Args&& ... args){
            expand_spawn(make_index_sequence<sizeof...(Args)>(), args...);
        }
        #undef inliner
    };
}

#endif
