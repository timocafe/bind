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

#ifndef BIND_CONTAINER_ARRAY_HPP
#define BIND_CONTAINER_ARRAY_HPP

namespace bind {
     
    template<class T, class Allocator> class array;
    namespace detail {
        template<class T, class Allocator>
        void fill_array(volatile bind::array<T,Allocator>& a, T& value){
            bind::array<T,Allocator>& a_ = const_cast<bind::array<T,Allocator>&>(a);
            for(size_t i = 0; i < a_.size(); ++i) a_[i] = value;
        }
        template<class T, class Allocator, class OtherAllocator = Allocator>
        void copy_array(volatile bind::array<T,Allocator>& dst, const bind::array<T,OtherAllocator>& src, const size_t& n){
            bind::array<T,Allocator>& dst_ = const_cast<bind::array<T,Allocator>&>(dst);
            for(size_t i = 0; i < n; ++i) dst_[i] = src[i];
        }
    }

    template<class T, class Allocator>
    array<T,Allocator>::array(size_t n, T value) : bind_allocator(n*sizeof(T)), size_(n) {
        this->fill(value);
    }

    template <typename T, class Allocator>
    array<T,Allocator>& array<T,Allocator>::operator = (const array& rhs){
        array c(rhs);
        this->swap(c);
        return *this;
    }

    template <typename T, class Allocator>
    template <class OtherAllocator>
    array<T,Allocator>& array<T,Allocator>::operator = (const array<T,OtherAllocator>& rhs){
        array resized(rhs.size());
        this->swap(resized);
        if(!bind::weak(rhs)) bind::cpu(detail::copy_array<T,Allocator,OtherAllocator>, *this, rhs, this->size_);
        return *this;
    }

    template<class T, class Allocator>
    void array<T,Allocator>::fill(T value){
        bind::cpu(detail::fill_array<T,Allocator>, *this, value);
    }

    template<typename T, class Allocator>
    void array<T,Allocator>::load() const {
        bind::load(*this); // shouldn't be needed
    }

    template<class T, class Allocator>
    void array<T,Allocator>::swap(array<T,Allocator>& r){
        std::swap(this->size_, r.size_);
        std::swap(this->bind_allocator.after->data, r.bind_allocator.after->data); // fixme
    }

    template<class T, class Allocator>
    size_t array<T,Allocator>::size() const {
        return size_;
    }

    template<class T, class Allocator>
    bool array<T,Allocator>::empty() const {
        return ((size() == 0) || bind::weak(*this));
    }

    template<class T, class Allocator>
    typename array<T,Allocator>::value_type* array<T,Allocator>::data(){
        return bind::delegated(*this).data;
    }

    template<class T, class Allocator>
    typename array<T,Allocator>::value_type& array<T,Allocator>::operator[](size_t i){
        return bind::delegated(*this).data[ i ];
    }

    template<typename T, class Allocator>
    typename array<T,Allocator>::value_type& array<T,Allocator>::at(size_type i){
        if(i >= size()) throw std::out_of_range("array::out_of_range");
        return (*this)[i];
    }

    template<typename T, class Allocator>
    typename array<T,Allocator>::value_type& array<T,Allocator>::front(){
        return (*this)[0];
    }

    template<typename T, class Allocator>
    typename array<T,Allocator>::value_type& array<T,Allocator>::back(){
        return (*this)[size()-1];
    }

    template<typename T, class Allocator>
    typename array<T,Allocator>::iterator array<T,Allocator>::begin(){
        return this->data();
    }

    template<typename T, class Allocator>
    typename array<T,Allocator>::iterator array<T,Allocator>::end(){
        return this->begin()+size();
    }

    template<class T, class Allocator>
    const typename array<T,Allocator>::value_type* array<T,Allocator>::data() const {
        return bind::delegated(*this).data;
    }

    template<class T, class Allocator>
    const typename array<T,Allocator>::value_type& array<T,Allocator>::operator[](size_t i) const {
        return bind::delegated(*this).data[ i ];
    }

    template<typename T, class Allocator>
    const typename array<T,Allocator>::value_type& array<T,Allocator>::at(size_type i) const {
        if(i >= size()) throw std::out_of_range("array::out_of_range");
        return (*this)[i];
    }

    template<typename T, class Allocator>
    const typename array<T,Allocator>::value_type& array<T,Allocator>::front() const {
        return (*this)[0];
    }

    template<typename T, class Allocator>
    const typename array<T,Allocator>::value_type& array<T,Allocator>::back() const {
        return (*this)[size()-1];
    }

    template<typename T, class Allocator>
    typename array<T,Allocator>::const_iterator array<T,Allocator>::cbegin() const {
        return this->data();
    }

    template<typename T, class Allocator>
    typename array<T,Allocator>::const_iterator array<T,Allocator>::cend() const {
        return this->begin()+size();
    }

}

#endif
