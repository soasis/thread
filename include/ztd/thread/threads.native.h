// =============================================================================
//
// ztd.thread
// Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.thread licenses may use this file in
// accordance with the commercial license agreement provided with the
// Software or, alternatively, in accordance with the terms contained in
// a written agreement between you and Shepherd's Oasis, LLC.
// For licensing terms and conditions see your agreement. For
// further information contact opensource@soasis.org.
//
// Apache License Version 2 Usage
// Alternatively, this file may be used under the terms of Apache License
// Version 2.0 (the "License") for non-commercial use; you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
// https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================ //

#pragma once

#ifndef ZTD_THREAD_THREADS_NATIVE_H
#define ZTD_THREAD_THREADS_NATIVE_H

#include <ztd/thread/version.h>

#include <ztd/idk/charN_t.h>

#if ZTD_IS_ON(ZTD_CXX)
#include <cstdint>
#include <ctime>
#include <climits>
#else
#include <stdint.h>
#include <time.h>
#include <limits.h>
#endif

#if ZTD_IS_ON(ZTD_HEADER_THREADS_H)
#if ZTD_IS_ON(ZTD_CXX) && ZTD_IS_ON(ZTD_HEADER_CTHREADS)
#include <cthreads>
#else
#include <threads.h>
#endif
#elif ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)
#include <ztd/thread/detail/threads.pthreads.h>
#elif ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
#include <ztd/thread/detail/threads.windows.h>
#else
#error "Unknown platform."
#endif

typedef
#if ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)
#if ZTD_IS_OFF(ZTD_HEADER_THREADS_H)
     pthread_t
#else
#if ZTD_IS_ON(ZTD_PLATFORM_MAC_OS)
     void*
#else
     unsigned long
#endif
#endif
#elif ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
     void*
#else
#error "Unknown platform."
#endif
          __ztdc_thrd_native_handle_t;

typedef
#if ZTD_IS_ON(ZTD_THREAD_INTEGER_THREAD_ID)
#if ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
     uint32_t
#else
     int32_t
#endif
#else
     uintptr_t
#endif
          __ztdc_thrd_id_t;

/// @brief The native handle type for this platform.
typedef __ztdc_thrd_native_handle_t ztdc_thrd_native_handle_t;

/// @brief The native id type for this platform.
typedef __ztdc_thrd_id_t ztdc_thrd_id_t;

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
/// @brief The null native handle value, which represents an invalid native handle.
const ztdc_thrd_native_handle_t ztdc_thrd_null_native_handle;

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
/// @brief The null id value, which represents an invalid id.
const ztdc_thrd_id_t ztdc_thrd_null_id;

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
/// @brief Returns the native handle out of `__thr`.
///
/// @param[in] __thr The thread to get the native, platform-specific handle for.
/// @param[inout] __p_thrd_handle The native handle object which represents `__thr`.
///
/// @return The native "handle" object which represents `__thr`. May be an invalid sentinel type if it can't fetch it
/// properly.
ztdc_thrd_native_handle_t ztdc_thrd_native_handle(thrd_t __thr);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
/// @brief Returns the native handle out of the current thread.
///
/// @return The native "handle" object which represents this thread. May be an invalid sentinel type if it can't fetch
/// it properly.
ztdc_thrd_native_handle_t ztdc_thrd_current_native_handle(void);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
/// @brief Returns the native id out of `__thr`.
///
/// @param[in] __thr The thread to get the native, platform-specific ID for.
///
/// @return The native "ID" object which represents `__thr`. May be an invalid sentinel type if `__thr` is not the
/// original object created by the thread.
ztdc_thrd_id_t ztdc_thrd_id(thrd_t __thr);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
/// @brief Returns the native id out of the current thread.
///
/// @return The native "ID" object which represents this thread. May be an invalid sentinel type if this.
ztdc_thrd_id_t ztdc_thrd_current_id(void);

#endif
