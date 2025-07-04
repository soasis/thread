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

#ifndef ZTD_THREAD_SOURCE_DETAIL_THREADS_IMPLEMENTATION_H
#define ZTD_THREAD_SOURCE_DETAIL_THREADS_IMPLEMENTATION_H

#include <ztd/thread/version.h>

#include <ztd/thread/threads.h>

#if ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)

#include <pthread.h>
#if ZTD_IS_ON(ZTD_HEADER_PTHREAD_NP_H)
#include <pthread_np.h>
#endif

#if ZTD_IS_ON(ZTD_HEADER_SYS_SYSCALL_H)
#include <sys/syscall.h>
#endif
#if ZTD_IS_ON(ZTD_HEADER_SYS_UIO_H)
#include <sys/uio.h>
#endif
#if ZTD_IS_ON(ZTD_HEADER_SYS_LWP_H)
#include <sys/lwp.h>
#endif
#if ZTD_IS_ON(ZTD_HEADER_SYS_THR_H)
#include <sys/thr.h>
#endif
#if ZTD_IS_ON(ZTD_HEADER_UNISTD_H)
// make sure to pull this in properly
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include <unistd.h>
#endif

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int __ztdc_pthreads_thrd_create_attrs_err(thrd_t* __thr, thrd_start_t __func, void* __func_arg, size_t __attrs_size,
     const ztdc_thrd_attr_kind** __attrs, ztdc_thrd_attr_err_func_t* __attr_err_func, void* __attr_err_func_arg);

#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)

#include <ztd/idk/detail/windows.h>

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int __ztdc_win32_thrd_create_attrs_err(thrd_t* __thr, thrd_start_t __func, void* __func_arg, size_t __attrs_size,
     const ztdc_thrd_attr_kind** __attrs, ztdc_thrd_attr_err_func_t* __attr_err_func, void* __attr_err_func_arg);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_INTERNAL_LINKAGE)
void** __ztdc_win32_handle_ptr(thrd_t* __thr);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_INTERNAL_LINKAGE)
uint32_t* __ztdc_win32_handle_id(thrd_t* __thr);

#else
#error "Unknown platform."
#endif

#endif
