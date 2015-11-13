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

#ifndef BIND_INTERFACE_MODIFIERS_SINGULAR
#define BIND_INTERFACE_MODIFIERS_SINGULAR

#define EXTRACT(var) T& var = *(T*)m->arguments[Arg];

namespace bind {
    using model::functor;

    template <typename T, bool Compact = false>
    struct singular_modifier {
        template<size_t Arg> static void deallocate(functor* ){ }
        template<size_t Arg> static bool pin(functor* ){ return false; }
        template<size_t Arg> static bool ready(functor* ){ return true; }
        template<size_t Arg> static void load(functor* m){ }
        template<size_t Arg> static T&   forward(functor* m){ EXTRACT(o); return o; }
        template<size_t Arg> static void apply_remote(T&){ }
        template<size_t Arg> static void apply_local(T& o, functor* m){
            m->arguments[Arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[Arg], &o, sizeof(T));
        }
        template<size_t Arg> static void apply (T& o, functor* m){
            m->arguments[Arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[Arg], &o, sizeof(T));
        }
        static constexpr bool ReferenceOnly = false;
    };

    template <typename T>
    struct singular_modifier<T, true> : public singular_modifier<T> {
        template<size_t Arg> static T& forward(functor* m){ return *(T*)&m->arguments[Arg]; }
        template<size_t Arg> static void apply_local(T& o, functor* m){ *(T*)&m->arguments[Arg] = o; }
        template<size_t Arg> static void apply(T& o, functor* m){
            *(T*)&m->arguments[Arg] = o;
        }
    };
}

#undef EXTRACT
#endif
