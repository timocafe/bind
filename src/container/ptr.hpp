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

#ifndef BIND_CONTAINER_PTR_HPP
#define BIND_CONTAINER_PTR_HPP

namespace bind {

    using model::transformable;
    using model::sizeof_transformable;

    template <typename T>
    class ptr {
    private:
        template <typename F> friend class ptr;
        template<typename S> 
        ptr& operator= (const S& val) = delete;
    public:
        typedef T value_type;

        T& operator* () const {
            return *desc;
        }
        void init(value_type val = T()){
            desc = new (memory::cpu::fixed::calloc<sizeof_transformable<T>()>()) transformable(val);
            valid = true;
        }
        template<typename S>
        void reuse(ptr<S>& f){
            desc = (transformable*)f.desc; // unsafe - proper convertion should be done
            valid = f.valid;
            f.desc = NULL; 
        }
        T load() const {
            if(!valid){
                bind::sync();
                valid = true;
            }
            return *desc;
        }
        const ptr<T>& unfold() const {
            return *this;
        }
        ptr<T>& unfold(){
            valid = false;
            return *this;
        }
       ~ptr(){ 
           if(desc) bind::destroy(desc); 
        }

        // constructors //
        ptr(){ 
            init();  
        }
        ptr(double val){ 
            init(val);
        }
        ptr(std::complex<double> val){
            init(val);
        }

        // copy //
        ptr(const ptr& f){
            init(f.load()); /* important */
        }
        template<typename S>
        ptr(const ptr<S>& f){
            init((T)f.load());
        }
        ptr& operator= (const ptr& f){
            *desc = f.load();
            return *this;
        }

        // move //
        ptr(ptr&& f){
            reuse(f);
        }
        template<typename S> 
        ptr(ptr<S>&& f){
            reuse(f);
        }
        ptr& operator= (ptr&& f){ 
            if(desc) bind::destroy(desc);
            reuse(f);
            return *this;
        }
    private:
        mutable bool valid;
    public:
        mutable transformable* desc;
    };

    template<class T>
    std::ostream& operator << (std::ostream& os, const ptr<T>& obj){
        os << obj.load();
        return os;
    }
}

#endif
