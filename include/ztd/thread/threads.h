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

#ifndef ZTD_THREAD_THREADS_H
#define ZTD_THREAD_THREADS_H

#include <ztd/thread/version.h>

#include <ztd/idk/charN_t.h>

#include <ztd/thread/threads.attr.h>

#if ZTD_IS_ON(ZTD_CXX)
#include <cstdint>
#else
#include <stdint.h>
#endif

#if ZTD_IS_ON(ZTD_HEADER_THREADS_H)
#if ZTD_IS_ON(ZTD_CXX) && ZTD_IS_ON(ZTD_HEADER_CTHREADS)
#include <cthreads>
#else
#include <threads.h>
#endif
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
#include <ztd/thread/detail/threads.windows.h>
#elif ZTD_IS_ON(ZTD_PLATFORM_PTHREADS)
#include <ztd/thread/detail/threads.pthreads.h>
#else
#error "Unknown platform."
#endif

#if ZTD_IS_OFF(ZTD_HEADER_THREADS_H)

typedef int(__thrd_start_t)(void*);
typedef __thrd_start_t* thrd_start_t;

typedef enum __thrd_error_code {
	thrd_success  = 0,
	thrd_nomem    = 1,
	thrd_timedout = 2,
	thrd_busy     = 3,
	thrd_error    = 4,
} __thrd_error_code;

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_create(thrd_t* __thr, thrd_start_t __func, void* __arg);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_join(thrd_t __thr, int* __res);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
ZTD_USE(ZTD_ATTR_NO_RETURN)
void thrd_exit(int __res);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_detach(thrd_t __thr);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_equal(thrd_t __left, thrd_t __right);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
thrd_t thrd_current(void);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
void thrd_yield(void);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_sleep(const struct timespec* __duration, struct timespec* __remaining);

#endif

/// @brief A function that is called when an attribute is not honored by an implementation. The first parameter is the
/// attribute passed into the function that does not work.
///
/// @param[in] __kind A pointer to the attribute's ztdc_thrd_attr_kind-typed first field.
/// @param[in] __err The error that the implementation provided. It can be any thread error except `thrd_success`.
/// @param[in] __userdata The user data passed in to the `thrd_create_arrs_err` function.
///
/// @remarks This function is always invoked on the same thread where the pointer to this function was provided for use
/// in a thread creation function.
///
/// @returns The return value is a thread error. Returning thrd_success means to ignore the error. Any other return
/// value means the function will stop and the thread creation will cease.
typedef int(ztdc_thrd_attr_err_func_t)(const ztdc_thrd_attr_kind* __kind, int __err, void* __userdata);


ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
/// @brief Starts a thread and configures that thread with the given thread attributes, if any. Any failure in the
/// thread attributes is completely and totally ignored by default. Such errors can be caught by using
/// ztdc_thrd_create_attrs_err.
///
/// @param __thr A pointer to the thread object that is populated with the information on success.
/// @param __func The function to invoke if the thread is successfully created and ran.
/// @param __arg The arg to provide to `__func` if it successfully starts.
/// @param __attrs_size The size of the `__attrs` array.
/// @param __attrs An array of pointers to the first member of either a standard or implementation-defined thread
/// attribute structure that is at least `__attrs_size` large. If `__attrs_size` is 0, this can be a null pointer.
/// Unknown attributes are skipped.
///
/// @return `thrd_success` on creation, and one of the thread error codes otherwise.
///
/// @remarks Equivalent to `ztdc_thrd_create_attrs_err(__thr, __func, __attrs_size, __attrs, NULL, NULL);`.
int ztdc_thrd_create_attrs(
     thrd_t* __thr, thrd_start_t __func, void* __arg, size_t __attrs_size, const ztdc_thrd_attr_kind** __attrs);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
/// @brief Starts a thread and configures that thread with the given thread attributes, if any. Any failure in the
/// threads is completely and totally ignored.
///
/// @param[out] __thr A pointer to the thread object that is populated with the information on success.
/// @param[in] __func The function to invoke if the thread is successfully created and ran.
/// @param[in] __arg The arg to provide to `__func` if it successfully starts.
/// @param[in] __attrs_size The size of the `__attrs` array.
/// @param[in] __attrs An array of pointers to the first member of either a standard or implementation-defined thread
/// attribute structure that is at least `__attrs_size` large. If `__attrs_size` is 0, this can be a null pointer.
/// Unknown attributes are skipped.
/// @param[in] __attr_err_func The function for handling when attributes cause errors. It is invoked on the same thread
/// as the invocation of the overall ztdc_thrd_create_attr_err invocation. Returning `thrd_success` means that the error
/// should be passed over and/or ignored for that given attribute. Otherwise, returning any other thread error results
/// in the function returning with that error code. If it is `NULL`, then the implementation assumes that any failed
/// application of a thread attribute should be passed over and ignored.
/// @param[in] __attr_err_func_arg The arg to provide to the attribute error function if its ever invoked.
///
/// @return `thrd_success` on creation, and one of the thread error codes otherwise.
///
/// @remarks Equivalent to `ztdc_thrd_create_attrs_err(__thr, __func, __attrs_size, __attrs, NULL, NULL);`.
int ztdc_thrd_create_attrs_err(thrd_t* __thr, thrd_start_t __func, void* __arg, size_t __attrs_size,
     const ztdc_thrd_attr_kind** __attrs, ztdc_thrd_attr_err_func_t* __attr_err_func, void* __attr_err_func_arg);


typedef
#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
     void*
#elif ZTD_IS_ON(ZTD_PLATFORM_PTHREADS)
#if ZTD_IS_OFF(ZTD_HEADER_THREADS_H)
     pthread_t
#else
#if ZTD_IS_ON(ZTD_PLATFORM_MAC_OS)
     void*
#else
     unsigned long
#endif
#endif
#else
#error "Unknown platform."
#endif
          __ztdc_thrd_native_handle_t;

typedef
#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
     uint32_t
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
/// @brief Returns the native handle out of `__thr`.
///
/// @param[in] __thr The thread to get the native, platform-specific handle for.
///
/// @return The native "handle" object which represents `__thr`. May be an invalid sentinel type if `__thr` is not the
/// original object created by the thread or retrieved by `thrd_current()`.
ztdc_thrd_native_handle_t ztdc_thrd_get_native_handle(thrd_t __thr);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
/// @brief Returns the native id out of `__thr`.
///
/// @param[in] __thr The thread to get the native, platform-specific ID for.
///
/// @return The native "ID" object which represents `__thr`. May be an invalid sentinel type if `__thr` is not the
/// original object created by the thread.
ztdc_thrd_id_t ztdc_thrd_get_id(thrd_t __thr);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_mcname(thrd_t __thr, size_t __buffer_size, char* __buffer);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_mcname(thrd_t __thr, size_t __buffer_size, char* __buffer);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_mwcname(thrd_t __thr, size_t __buffer_size, ztd_wchar_t* __buffer);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_c8name(thrd_t __thr, size_t __buffer_size, ztd_char8_t* __buffer);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_c16name(thrd_t __thr, size_t __buffer_size, ztd_char16_t* __buffer);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_c32name(thrd_t __thr, size_t __buffer_size, ztd_char32_t* __buffer);

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_name(thrd_t __thr, size_t __buffer_size, void* __buffer);

#endif
