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

#ifndef BIND_CONTAINER_ARRAY_H
#define BIND_CONTAINER_ARRAY_H

namespace bind {

    template <class T, class Allocator = bind::allocator>
    class array {
    public:
        void* operator new (size_t size, void* ptr){ return ptr; }
        void  operator delete (void*, void*){ /* doesn't throw */ }
        void* operator new (size_t sz){ return memory::cpu::standard::malloc<sizeof(array)>(); }
        void operator delete (void* ptr){ memory::cpu::standard::free(ptr); }
    public:
        using allocator_type = Allocator;
        using value_type = T;
        using size_type = size_t;
        using const_iterator = iterator<const array>;
        using iterator = iterator<array>;
        explicit array(){}

        explicit array(size_t n);
        array(const array& a) = default;
        array& operator = (const array& rhs);
        template<class OtherAllocator>
        array& operator = (const array<T,OtherAllocator>& rhs);

        void fill(T value);
        void swap(array<T,Allocator>& r);
        size_t size() const;
        bool empty() const;

        value_type* data();
        value_type& operator[](size_t i);
        value_type& at(size_type i);
        value_type& front();
        value_type& back();
        iterator begin();
        iterator end();

        const value_type* data() const;
        const value_type& operator[](size_t i) const;
        const value_type& at(size_type i) const;
        const value_type& front() const;
        const value_type& back() const;
        const_iterator cbegin() const;
        const_iterator cend() const;
    private:
        mutable size_t size_;
    public:
        mutable allocator_type allocator_;
    };

}

#endif
