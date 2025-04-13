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


#include <ztd/thread/threads.h>

#include <ztd/thread/detail/threads.implementation.h>

#include <ztd/idk/static_assert.h>
#include <ztd/idk/size.h>

inline static int __ztdc_ignore_all_thrd_errors(const ztdc_thrd_attr_kind* __kind, int __err, void* __userdata) {
	(void)__kind;
	(void)__err;
	(void)__userdata;
	return thrd_success;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_create_attrs(
     thrd_t* __thr, thrd_start_t __func, void* __func_arg, size_t __attrs_size, const ztdc_thrd_attr_kind** __attrs) {
	return ztdc_thrd_create_attrs_err(
	     __thr, __func, __func_arg, __attrs_size, __attrs, __ztdc_ignore_all_thrd_errors, NULL);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_create_attrs_err(thrd_t* __thr, thrd_start_t __func, void* __func_arg, size_t __attrs_size,
     const ztdc_thrd_attr_kind** __attrs, ztdc_thrd_attr_err_func_t* __attr_err_func, void* __attr_err_func_arg) {
#if ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
#elif ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)
#if ZTD_IS_ON(ZTD_HEADER_THREADS_H) || ZTD_IS_ON(ZTD_HEADER_CTHREADS)
	ztdc_static_assert(sizeof(thrd_t) >= sizeof(pthread_t), "thrd_t is not the same size as pthread_t");
#endif
#endif
	if (!__thr) {
		return thrd_error;
	}
	if (!__func) {
		return thrd_success;
	}
	if (!__attr_err_func) {
		__attr_err_func = __ztdc_ignore_all_thrd_errors;
	}
#if ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)
	return __ztdc_pthreads_thrd_create_attrs_err(
	     __thr, __func, __func_arg, __attrs_size, __attrs, __attr_err_func, __attr_err_func_arg);
#elif ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
	return __ztdc_win32_thrd_create_attrs_err(
	     __thr, __func, __func_arg, __attrs_size, __attrs, __attr_err_func, __attr_err_func_arg);
#else
#error "Unknown platform."
#endif
}


ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
ztdc_thrd_native_handle_t ztdc_thrd_get_native_handle(thrd_t __thr) {
#if ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)
	return __thr;
#elif ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
	return *__ztdc_win32_handle_ptr(&__thr);
#else
#error "Unknown platform."
#endif
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
ztdc_thrd_id_t ztdc_thrd_get_id(thrd_t __thr) {
#if ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)
	return (ztdc_thrd_id_t)__thr;
#elif ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
	return *__ztdc_win32_handle_id(&__thr);
#else
#error "Unknown platform."
#endif
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_native_name(thrd_t __thr, size_t __buffer_size, void* __buffer) {
#if ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)
	return ztdc_thrd_get_c8name(__thr, __buffer_size / sizeof(ztd_char8_t), (ztd_char8_t*)__buffer);
#elif ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
	return ztdc_thrd_get_mwcname(__thr, __buffer_size / sizeof(ztd_wchar_t), (ztd_wchar_t*)__buffer);
#else
#error "Unknown platform."
#endif
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_native_name(thrd_t __thr, const void* __buffer) {
#if ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
	return ztdc_thrd_set_mwcname(__thr, (const ztd_wchar_t*)__buffer);
#elif ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)
	return ztdc_thrd_set_c8name(__thr, (const ztd_char8_t*)__buffer);
#else
#error "Unknown platform."
#endif
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_native_name_sized(thrd_t __thr, size_t __buffer_size, const void* __buffer) {
#if ZTD_IS_ON(ZTD_THREAD_ANY_WIN32_BASED)
	return ztdc_thrd_set_mwcname_sized(__thr, __buffer_size / sizeof(ztd_wchar_t), (const ztd_wchar_t*)__buffer);
#elif ZTD_IS_ON(ZTD_THREAD_PTHREAD_BASED)
	return ztdc_thrd_set_c8name_sized(__thr, __buffer_size / sizeof(char), (const ztd_char8_t*)__buffer);
#else
#error "Unknown platform."
#endif
}

#if ZTD_IS_OFF(ZTD_THREAD_PTHREAD_BASED)
ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_mcname(thrd_t __thr, const char* __buffer) {
	return ztdc_thrd_set_mcname_sized(__thr, ztdc_c_string_ptr_size(__buffer), __buffer);
}
#endif

#if ZTD_IS_OFF(ZTD_THREAD_ANY_WIN32_BASED)
ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_mwcname(thrd_t __thr, const ztd_wchar_t* __buffer) {
	return ztdc_thrd_set_mwcname_sized(__thr, ztdc_c_string_ptr_size(__buffer), __buffer);
}
#endif

#if ZTD_IS_OFF(ZTD_THREAD_PTHREAD_BASED)
ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_c8name(thrd_t __thr, const ztd_char8_t* __buffer) {
	return ztdc_thrd_set_c8name_sized(__thr, ztdc_c_string_ptr_size(__buffer), __buffer);
}
#endif

#if ZTD_IS_OFF(ZTD_THREAD_ANY_WIN32_BASED)
ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_c16name(thrd_t __thr, const ztd_char16_t* __buffer) {
	return ztdc_thrd_set_c16name_sized(__thr, ztdc_c_string_ptr_size(__buffer), __buffer);
}
#endif

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_c32name(thrd_t __thr, const ztd_char32_t* __buffer) {
	return ztdc_thrd_set_c32name_sized(__thr, ztdc_c_string_ptr_size(__buffer), __buffer);
}
