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
        descriptor(size_t e, types::id_type t = types::none) : extent(e), type(t), tmp(false) {}

        template<class Device>
        bool conserves(descriptor& p){
            return (p.type == types::cpu::standard || type == types::cpu::bulk);
        }
        template<class Device>
        bool complies(){
            return true;
        }
        void free(void* ptr){
            if(ptr == NULL || type == types::none) return;
            if(type == types::cpu::standard){
                cpu::standard::free(ptr);
                type = types::none;
            }
        }
        template<class Device>
        void* malloc(){
            if(type == types::cpu::bulk){
                void* ptr = cpu::data_bulk::soft_malloc(extent);
                if(ptr) return ptr;
            }
            type = types::cpu::standard;
            return cpu::standard::malloc(extent);
        }
        template<class Memory>
        void* hard_malloc(){
            type = Memory::type;
            return Memory::malloc(extent);
        }
        template<class Device>
        void* calloc(){
            void* m = malloc<Device>(); memset(m, 0, extent); return m; // should be memory-specific
        }
        template<class Device>
        void memmove(void* ptr){
        }
        template<class Device>
        void memcpy(void* dst, void* src, descriptor& src_desc){
            std::memcpy(dst, src, src_desc.extent);
        }
        void reuse(descriptor& d){
            type = d.type;
            d.type = types::none;
        }
        void temporary(bool t){
            if(t) type = types::cpu::bulk;
            else type = types::cpu::standard;
        }
    public:
        const size_t extent;
    private:
        types::id_type type;
        bool tmp;
    };

} }

#endif
