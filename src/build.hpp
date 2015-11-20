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
// {{{ required packages
#if defined(BIND_REQUIRE_MPI) && !defined(MPI_VERSION)
#pragma message("Warning: mpi.h is required but wasn't included.")
#include <mpi.h>
#endif
#if defined(BIND_REQUIRE_CUDA) && !defined(CUDART_VERSION)
#pragma message("Warning: cuda_runtime.h is required but wasn't included.")
#include <cuda_runtime.h>
#endif
// }}}
// {{{ system includes
#include <complex>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <cstring>
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
#include "utils/index_sequence.hpp"
#include "utils/io.hpp"
#include "utils/guard.hpp"
#include "utils/rank_t.hpp"
// }}}
// {{{ memory package
#include "memory/types.h"
#include "memory/detail/factory.hpp"
#include "memory/detail/region.hpp"
    // {{{ memory::cpu package
    #include "memory/cpu/bulk.h"
    #include "memory/cpu/instr_bulk.hpp"
    #include "memory/cpu/standard.hpp"
    #include "memory/cpu/new.hpp"
    // }}}
    // {{{ memory::gpu package
    #ifdef CUDART_VERSION
    #include "memory/gpu/pinned.hpp"
    #include "memory/gpu/standard.hpp"
    #endif
    // }}}
#include "memory/descriptor.hpp"
#include "memory/hub.hpp"
// }}}
// {{{ model package
#include "model/device.hpp"
#include "model/locality.hpp"
#include "model/functor.hpp"
#include "model/revision.hpp"
#include "model/history.hpp"
#include "model/any.hpp"
// }}}
// {{{ transport package (requires :model)
#ifdef MPI_VERSION
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
#define BIND_CHANNEL_NAME nop
#include "transport/nop/channel.hpp"
#endif
#ifdef CUDART_VERSION
#include "transport/cuda/channel.hpp"
#endif
// }}}
// {{{ core package (requires :model :transport)
#include "core/collector.hpp"
#include "core/node.h"
#include "core/controller.h"
#ifdef CUDART_VERSION
#include "core/transfer.h"
#include "core/transfer.hpp"
#endif
#include "core/get.h"
#include "core/get.hpp"
#include "core/set.h"
#include "core/set.hpp"
#include "core/hub.hpp"
#include "core/controller.hpp"
#include "core/node.hpp"
// }}}
// {{{ interface package (requires :model :transport :core)
#include "interface/shortcuts.hpp"
#include "interface/modifiers/singular.hpp"
#include "interface/modifiers/shared_ptr.hpp"
#include "interface/modifiers/iterator.hpp"
#include "interface/modifiers/versioned.hpp"
#include "interface/modifiers/dispatch.hpp"
#include "interface/snapshot.hpp"
#include "interface/kernel_inliner.hpp"
#include "interface/kernel.hpp"
#include "interface/lambda.hpp"
// }}}
// {{{ container package (requires :*)
#include "container/smart_ptr.hpp"
#include "container/proxy_iterator.hpp"
#include "container/array.hpp"
#include "container/block.hpp"
// }}}
#ifdef BIND_NO_DEBUG
#undef NDEBUG
#endif
#endif
