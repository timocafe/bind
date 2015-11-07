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

#ifndef BIND_INTERFACE_MODIFIERS_ITERATOR
#define BIND_INTERFACE_MODIFIERS_ITERATOR

#define EXTRACT(var) T& var = *(T*)m->arguments[arg];

namespace bind {
    using model::functor;
    template <typename T> struct modifier;

    template <typename T>
    struct iterator_modifier : public singular_modifier<T> {
        typedef typename modifier<typename T::container_type>::type type;
        typedef typename T::container_type container_type;

        template<size_t arg> 
        static void deallocate(functor* m){
            EXTRACT(o); type::deallocate_(*o.container);
        }
        template<size_t arg>
        static void apply_remote(T& o){
            type::template apply_remote<arg>(*o.container);
        }
        template<size_t arg>
        static void apply_local(T& o, functor* m){
            type::template apply_local<arg>(*o.container, m);
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &o, sizeof(T));
            var->container = (container_type*)m->arguments[arg]; m->arguments[arg] = (void*)var;
        }
        template<size_t arg>
        static void apply(T& o, functor* m){
            type::template apply<arg>(*o.container, m);
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &o, sizeof(T));
            var->container = (container_type*)m->arguments[arg]; m->arguments[arg] = (void*)var;
        }
        template<size_t arg>
        static void load(functor* m){
            EXTRACT(o); revise(*o.container);
        }
        template<size_t arg> 
        static bool pin(functor* m){ 
            EXTRACT(o); return type::pin_(*o.container, m);
        }
        template<size_t arg> 
        static bool ready(functor* m){
            EXTRACT(o); return type::ready_(*o.container, m);
        }
    };
}

#undef EXTRACT
#endif
