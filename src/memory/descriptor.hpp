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
        typedef int memory_id_type;

        descriptor(size_t e, memory_id_type r = types::cpu::standard) : extent(e), signature(r), persistency(1), crefs(1) {}

        void protect(){
            if(!(persistency++)) signature = types::cpu::standard;
        }
        void weaken(){
            if(!(--persistency)) signature = types::cpu::bulk;
        }
        void reuse(descriptor& d){
            signature = d.signature;
            d.signature = types::none;
        }
        bool conserves(descriptor& p){
            assert(p.signature != types::none && signature != types::none);
            return (!p.bulked() || bulked());
        }
        bool bulked(){
            return (signature == types::cpu::bulk);
        }
        void* malloc(){
            assert(signature != types::none);
            if(signature == types::cpu::bulk){
                void* ptr = cpu::data_bulk::soft_malloc(extent);
                if(ptr) return ptr;
                signature = types::cpu::standard;
            }
            return cpu::standard::malloc(extent);
        }
        template<class Memory>
        void* hard_malloc(){
            signature = Memory::type;
            return Memory::malloc(extent);
        }
        void free(void* ptr){ 
            if(ptr == NULL || signature == types::none) return;
            if(signature == types::cpu::standard) cpu::standard::free(ptr);
        }

        size_t extent;
        memory_id_type signature;
        int persistency;
        int crefs;
    };

} }

#endif
