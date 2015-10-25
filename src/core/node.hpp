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

#ifndef BIND_CORE_NODE_HPP
#define BIND_CORE_NODE_HPP

namespace bind {

    // {{{ primary node-class

    inline node::~node(){
        if(!this->controller) return;
        bind::select().deactivate(this);
    }
    inline node::node(scope::const_iterator it){
        if(! (this->controller = bind::select().activate(this)) ) return;
        this->rank = (*it) % this->controller->get_num_procs();
        this->state = (this->rank == controller->get_rank()) ? locality::local : locality::remote;
    }
    inline bool node::remote() const {
        return (state == locality::remote);
    }
    inline bool node::local() const {
        return (state == locality::local);
    }
    inline bool node::common() const {
        return (state == locality::common);
    }
    inline rank_t node::which() const {
        return this->rank;
    }

    // }}}
    // {{{ node's special case: everyone does the same

    inline node_zero::node_zero(typename node::controller_type* c){
        this->controller = c;
        this->rank = controller->get_shared_rank();
        this->state = locality::common;
    }

    // }}}
}

#endif
