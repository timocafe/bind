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

#ifndef BIND
#define BIND
#ifndef NDEBUG
#define NDEBUG
#define BIND_NO_DEBUG
#endif
// {{{ system includes
#ifdef BIND_MPI
#include <mpi.h>
#endif
#include <complex>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <limits>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <list>
#include <memory.h>
#include <stdarg.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <algorithm>
#include <execinfo.h>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include <utility>
#include <atomic>
#include <tuple>
#include <sys/mman.h>
#include <chrono>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
// }}}
// {{{ utils package
#include "utils/rss.hpp"
#include "utils/index_sequence.hpp"
#include "utils/io.hpp"
#include "utils/mutex.hpp"
#include "utils/rank_t.hpp"
// }}}
// {{{ memory package
#include "memory/types.h"
#include "memory/factory.hpp"
#include "memory/region.hpp"
    // {{{ memory::cpu package
    #include "memory/cpu/bulk.h"
    #include "memory/cpu/data_bulk.hpp"
    #include "memory/cpu/comm_bulk.hpp"
    #include "memory/cpu/instr_bulk.hpp"
    #include "memory/cpu/standard.hpp"
    #include "memory/cpu/fixed.hpp"
    #include "memory/cpu/new.hpp"
    // }}}
    // {{{ memory::gpu package
    // }}}
#include "memory/delegated.h"
#include "memory/descriptor.hpp"
// }}}
// {{{ model package
#include "model/locality.hpp"
#include "model/functor.hpp"
#include "model/revision.hpp"
#include "model/history.hpp"
#include "model/any.hpp"
// }}}
// {{{ transport package (requires :model)
#ifdef BIND_MPI
#define BIND_CHANNEL_NAME mpi
#include "transport/mpi/group.hpp"
#include "transport/mpi/tree.hpp"
#include "transport/mpi/channel.h"
#include "transport/mpi/request.h"
#include "transport/mpi/collective.h"
#include "transport/mpi/request.hpp"
#include "transport/mpi/channel.hpp"
#include "transport/mpi/collective.hpp"
#else
#include "transport/nop/channel.hpp"
#endif
// }}}
// {{{ core package (requires :model :transport)
#include "core/collector.h"
#include "core/collector.hpp"
#include "core/node.h"
#include "core/controller.h"
#include "core/get.h"
#include "core/set.h"
#include "core/controller.hpp"
#include "core/get.hpp"
#include "core/set.hpp"
#include "core/node.hpp"
// }}}
// {{{ interface package (requires :model :transport :core)
#include "interface/shortcuts.hpp"
#include "interface/typed.hpp"
#include "interface/allocator.hpp"
#include "interface/kernel_inliner.hpp"
#include "interface/kernel.hpp"
#include "interface/access.hpp"
#include "interface/lambda.hpp"
// }}}
// {{{ bonus container package (requires :*)
#include "container/ptr.hpp"
#include "container/block.hpp"
#include "container/vector.h"
#include "container/vector.hpp"
#include "container/vector_async.h"
#include "container/vector_async.hpp"
// }}}
// {{{ bonus utils package (requires :*)
#include "utils/timer.hpp"
// }}}
#ifdef BIND_NO_DEBUG
#undef NDEBUG
#endif
#endif
