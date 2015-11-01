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

#ifndef BIND_INTERFACE_MODIFIERS_PTR
#define BIND_INTERFACE_MODIFIERS_PTR

#define EXTRACT(var) T& var = *(T*)m->arguments[arg];

namespace bind {
    using model::functor;

    template <typename T> struct const_ptr_modifier : public singular_modifier<T> {
        template<size_t arg> static bool ready(functor* m){
            EXTRACT(o);
            if(o.impl->origin && o.impl->origin->generator != NULL) return false;
            return (o.impl->generator == m || o.impl->generator == NULL);
        }
        template<size_t arg> static bool pin(functor* m){
            EXTRACT(o);
            if(o.impl->generator == NULL) return false;
            (o.impl->generator.load())->queue(m);
            return true;
        }
        static constexpr bool ReferenceOnly = true;
    };

    template <typename T> struct ptr_modifier : public const_ptr_modifier<T> {
        template<size_t arg> static void deallocate(functor* m){
            EXTRACT(o); o.impl->complete();
        }
        template<size_t arg> static bool pin(functor* m){
            EXTRACT(o);
            if(!o.impl->origin || o.impl->origin->generator == NULL) return false;
            (o.impl->origin->generator.load())->queue(m);
            return true;
        }
        template<size_t arg> static void apply_remote(T& o){
            o.resit();
            bind::select().rsync(o.impl);
        }
        template<size_t arg> static void apply_local(const T& o, functor* m){
            if(o.impl->generator != m){
                o.resit();
                o.impl->generator = m;
            }
            bind::select().lsync(o.impl);
            m->arguments[arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[arg], &o, sizeof(T)); 
        }
        template<size_t arg> static void apply(const T& o, functor* m){
            if(o.impl->generator != m){
                o.resit();
                o.impl->generator = m;
            }
            m->arguments[arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[arg], &o, sizeof(T)); 
        }
        template<size_t arg> static T& load(functor* m){
            EXTRACT(o);
            if(o.impl->origin){
                *o.impl = (typename T::element_type&)*o.impl->origin;
                o.impl->origin = NULL;
            }
            return o;
        }
    };
}

#undef EXTRACT
#endif
