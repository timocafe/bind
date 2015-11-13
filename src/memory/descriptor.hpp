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

#ifndef BIND_MEMORY_DESCRIPTOR
#define BIND_MEMORY_DESCRIPTOR

namespace bind { namespace memory {

    struct descriptor {
        descriptor(size_t e, types::id_type t = types::cpu::standard) : extent(e), type(t), persistency(1), crefs(1) {}

        void protect(){
            if(!(persistency++)) type = types::cpu::standard;
        }
        void weaken(){
            if(!(--persistency)) type = types::cpu::bulk;
        }
        void reuse(descriptor& d){
            type = d.type;
            d.type = types::none;
        }
        bool conserves(descriptor& p){
            assert(p.type != types::none && type != types::none);
            return (p.type != types::cpu::bulk || type == types::cpu::bulk);
        }
        void* malloc(){
            assert(type != types::none);
            if(type == types::cpu::bulk){
                void* ptr = cpu::data_bulk::soft_malloc(extent);
                if(ptr) return ptr;
                type = types::cpu::standard;
            }
            return cpu::standard::malloc(extent);
        }
        template<class Memory>
        void* hard_malloc(){
            type = Memory::type;
            return Memory::malloc(extent);
        }
        void* calloc(){
            void* m = malloc(); memset(m, 0, extent); return m; // should be memory-specific
        }
        void free(void* ptr){ 
            if(ptr == NULL || type == types::none) return;
            if(type == types::cpu::standard) cpu::standard::free(ptr);
        }

        size_t extent;
        types::id_type type;
        int persistency;
        int crefs;
    };

} }

#endif
