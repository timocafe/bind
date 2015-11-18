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

#ifndef BIND_UTILS_MUTEX
#define BIND_UTILS_MUTEX

namespace bind { 

    class guard_once {
    public:
        guard_once() : once(false) { }
        bool operator()(){ if(!once){ once = true; return true; } return false; }
    private:
        bool once;
    };

    template <typename M>
    class guard {
    private:
        M& mtx;
        guard(const guard &);
        void operator= (const guard &);
    public:
        explicit guard(M& nmtx) : mtx(nmtx){ mtx.lock(); }
        ~guard(){ mtx.unlock(); }
    };

    class mutex {
    private:
        pthread_mutex_t m;
    public:
        mutex(mutex const&) = delete;
        mutex& operator= (mutex const&) = delete;

        mutex(){
            int const res = pthread_mutex_init(&m,NULL);
            assert(res == 0);
        }
       ~mutex(){
            int ret;
            do{ ret = pthread_mutex_destroy(&m);
            } while(ret == EINTR);
        }
        void lock(){
            int res;
            do{ res = pthread_mutex_lock(&m);
            }while (res == EINTR);
            assert(res == 0);
        }
        void unlock(){
            int res;
            do{ res = pthread_mutex_unlock(&m);
            } while(res == EINTR);
            assert(res == 0);
        }
        bool try_lock(){
            int res;
            do{ res = pthread_mutex_trylock(&m);
            } while(res == EINTR);
            if(res == EBUSY) return false;
            return !res;
        }
    };
}

#endif
