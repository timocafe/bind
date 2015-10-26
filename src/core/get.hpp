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

namespace bind { namespace core {

    // {{{ any

    inline void get<any>::spawn(any& t){
        bind::select().queue(new get(t));
    }
    inline get<any>::get(any& t){
        handle = bind::select().get_channel().bcast(t, bind::nodes::which());
    }
    inline bool get<any>::ready(){
        return handle->test();
    }
    inline void get<any>::invoke(){}

    // }}}
    // {{{ revision

    inline void get<revision>::spawn(revision& r){
        get*& transfer = (get*&)r.assist.second;
        if(bind::select().update(r)) transfer = new get(r);
        *transfer += bind::nodes::which();
    }
    inline get<revision>::get(revision& r) : t(r) {
        handle = bind::select().get_channel().get(t);
        t.invalidate();
    }
    inline void get<revision>::operator += (rank_t rank){
        handle->append(rank);
        if(handle->involved() && !t.valid()){
            t.use();
            t.generator = this;
            t.embed(t.spec.hard_malloc<memory::cpu::comm_bulk>()); 
            bind::select().queue(this);
        }
    }
    inline bool get<revision>::ready(){
        return handle->test();
    }
    inline void get<revision>::invoke(){
        bind::select().squeeze(&t);
        t.release();
        t.complete();
    }

    // }}}

} }
