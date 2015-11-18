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

#ifndef BIND_CORE_HUB_HPP
#define BIND_CORE_HUB_HPP

namespace bind { namespace transport {
    using model::revision;
    using model::any;

    template<typename T, device D, locality L> struct hub;
    template<locality L> class hub_mpi;

    // {{{ hub for model::any
    template<device D>
    struct hub<any, D, locality::local> {
        static void sync(any* v){
            if(bind::nodes::size() == 1) return;
            core::set<any>::spawn(*v);
        }
    };
    template<device D>
    struct hub<any, D, locality::remote> {
        static void sync(any* v){
            core::get<any>::spawn(*v);
        }
    };
    // }}}
    // {{{ mpi-only hub
    template<>
    struct hub_mpi<locality::common> {
        static void sync(revision* r){
            if(bind::nodes::size() == 1) return; // serial
            if(model::common(r)) return;
            if(model::local(r)) core::set<revision>::spawn(*r);
            else core::get<revision>::spawn(*r);
        }
    };
    template<>
    struct hub_mpi<locality::local> {
        static void sync(revision* r){
            if(model::remote(r)) core::get<revision>::spawn(*r);
        }
    };
    template<>
    struct hub_mpi<locality::remote> {
        static void sync(revision* r){
            if(r->owner == bind::nodes::which_() || model::common(r)) return;
            if(model::local(r)) core::set<revision>::spawn(*r);
            else core::get<revision>::spawn(*r); // assist
        }
    };
    // }}}
    #ifndef CUDART_VERSION
    // {{{ default hub for model::revision
    template<device D, locality L>
    struct hub<revision, D, L> {
        static void sync(revision* r, revision*){
            hub_mpi<L>::sync(r);
        }
    };
    // }}}
    #else
    // {{{ device-aware hub for model::revision
    // {{{ cpu
    template<locality L>
    struct hub<revision, device::cpu, L> {
        static void sync(revision*& r, revision*& s){
            if(model::gpu(r)){
                if(!s){
                    s = r->clone<device::cpu>(); if(!model::remote(r))
                    cuda::transfer<device::gpu, device::cpu>::spawn(*r, *s);
                }
                std::swap(r, s);
            }
            hub_mpi<L>::sync(r);
        }
    };
    template<>
    struct hub<revision, device::cpu, locality::remote> {
        static void sync(revision*& r, revision*& s){
            if(model::gpu(r)){
                if(!s){
                    s = r->clone<device::cpu>(); if(model::local(r))
                    cuda::transfer<device::gpu, device::cpu>::spawn(*r, *s);
                }
                std::swap(r, s);
            }
            hub_mpi<locality::remote>::sync(r);
        }
    };
    // }}}
    // {{{ gpu
    template<>
    struct hub<revision, device::gpu, locality::common> {
        static void sync(revision*& r, revision*& s){
            if(model::cpu(r)){
                hub_mpi<locality::common>::sync(r);
                if(!s){
                    s = r->clone<device::gpu>();
                    cuda::transfer<device::cpu, device::gpu>::spawn(*r, *s);
                }
                std::swap(r, s);
            }else{
                if(model::common(r)) return;
                if(!s){
                    s = r->clone<device::cpu>();        if(model::local(r))  cuda::transfer<device::gpu, device::cpu>::spawn(*r, *s);
                    hub_mpi<locality::common>::sync(s); if(model::remote(r)) cuda::transfer<device::cpu, device::gpu>::spawn(*s, *r);
                }else
                    hub_mpi<locality::common>::sync(s); // legacy refresh
            }
        }
    };
    template<>
    struct hub<revision, device::gpu, locality::local> {
        static void sync(revision*& r, revision*& s){
            if(model::cpu(r)){
                hub_mpi<locality::local>::sync(r);
                if(!s){
                    s = r->clone<device::gpu>();
                    cuda::transfer<device::cpu, device::gpu>::spawn(*r, *s);
                }
                std::swap(r, s);
            }else{
                if(!model::remote(r)) return;
                if(!s){
                    s = r->clone<device::cpu>(); hub_mpi<locality::local>::sync(s);
                    cuda::transfer<device::cpu, device::gpu>::spawn(*s, *r);
                }else
                    hub_mpi<locality::local>::sync(s); // legacy refresh
            }
        }
    };
    template<>
    struct hub<revision, device::gpu, locality::remote> {
        static void sync(revision*& r, revision*& s){
            hub<revision, device::cpu, locality::remote>::sync(r, s);
        }
    };
    // }}}
    // }}}
    #endif
} }

#endif
