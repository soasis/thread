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

#include <ztd/thread/version.h>

#if ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)

#include <ztd/thread/threads.h>

#include <ztd/idk/static_assert.h>
#include <ztd/idk/size.h>
#include <ztd/cuneicode.h>

#include <ztd/thread/detail/threads.implementation.h>

#if ZTD_IS_ON(ZTD_CXX)
#include <cstdlib>
#else
#include <stdlib.h>
#include <stdbool.h>
#endif

static inline int __ztdc_win32_to_thread_error(int __code) {
	switch (__code) {
	case ERROR_SUCCESS:
		return thrd_success;
	case WAIT_TIMEOUT:
	case ERROR_TIMEOUT:
	case ERROR_SEM_TIMEOUT:
		return thrd_timedout;
	case ERROR_NOT_READY:
	case ERROR_LOCK_FAILED:
	case ERROR_LOCK_VIOLATION:
	case ERROR_LOCKED:
		return thrd_busy;
	case ERROR_INVALID_ACCESS:
	case ERROR_INVALID_DATA:
	case ERROR_BAD_LENGTH:
		return thrd_error;
	case ERROR_NOT_ENOUGH_MEMORY:
	case ERROR_NOT_ENOUGH_SERVER_MEMORY:
	case ERROR_OUTOFMEMORY:
		return thrd_nomem;
	default:
		return thrd_error;
	}
}

static inline int __ztdc_hresult_to_thread_error(HRESULT __code) {
	return __ztdc_win32_to_thread_error(HRESULT_CODE(__code));
}

int __ztdc_xthreads_to_thread_error(int __code) {
	// fairly direct, one-to-one translation
	return __code;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_INTERNAL_LINKAGE)
void** __ztdc_win32_handle_ptr(thrd_t* __thr) {
#if ZTD_IS_ON(ZTD_HEADER_THREADS_H)
	return &__thr->_Handle;
#elif ZTD_IS_ON(ZTD_HEADER_XTHREADS_H)
	return &__thr->_Hnd;
#else
	return &__thr->__handle;
#endif
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_INTERNAL_LINKAGE)
uint32_t* __ztdc_win32_handle_id(thrd_t* __thr) {
#if ZTD_IS_ON(ZTD_HEADER_THREADS_H)
	return &__thr->_Tid;
#elif ZTD_IS_ON(ZTD_HEADER_XTHREADS_H)
	return &__thr->_Id;
#else
	return &__thr->__id;
#endif
}

#if ZTD_IS_OFF(ZTD_HEADER_THREADS_H)
#if ZTD_IS_ON(ZTD_HEADER_XTHREADS_H)

#include <time.h>

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_create(thrd_t* __thr, thrd_start_t __func, void* __func_arg) {
	return ztdc_thrd_create_attrs(__thr, __func, __func_arg, 0, nullptr);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE) ZTD_USE(ZTD_THREAD_API_LINKAGE) int thrd_join(thrd_t __thr, int* __res) {
	return __ztdc_xthreads_to_thread_error(_Thrd_join(__thr, __res));
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
ZTD_USE(ZTD_ATTR_NO_RETURN)
void thrd_exit(int __res) {
	// TODO: this will not invoke C++ destructors. Fine for C, bad for C++.
	ExitThread((DWORD)__res);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_detach(thrd_t __thr) {
	return __ztdc_xthreads_to_thread_error(_Thrd_detach(__thr));
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_equal(thrd_t __left, thrd_t __right) {
	return *__ztdc_win32_handle_id(__left) == *__ztdc_win32_handle_id(__right)
	     && *__ztdc_win32_handle_ptr(__left) == *__ztdc_win32_handle_ptr(__right);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
thrd_t thrd_current(void) {
	thrd_t __current_thr = {
		GetCurrentThread(),
		(uint32_t)GetCurrentThreadId(),
	};
	return __current_thr;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
void thrd_yield(void) {
	_Thrd_yield();
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_sleep(const struct timespec* __duration, struct timespec* __remaining) {
	unsigned long __ms = __duration->tv_sec * 1000;
	__ms += __duration->tv_sec / 1000000;
	LARGE_INTEGER __before_time;
	LARGE_INTEGER __after_time;
	BOOL __elapsed_valid;
	if (__remaining) {
		__elapsed_valid = QueryPerformanceCounter(&__before_time);
	}
	_Thrd_sleep_for(__ms);
	if (!__remaining) {
		return thrd_success;
	}
	__elapsed_valid = QueryPerformanceCounter(&__after_time);
	if (!__elapsed_valid) {
		return thrd_success;
	}
	LARGE_INTEGER __frequency;
	__elapsed_valid = QueryPerformanceFrequency(&__frequency);
	if (!__elapsed_valid) {
		return thrd_success;
	}
	ULONGLONG __time_diff = __after_time.QuadPart - __before_time.QuadPart;
	// not a clue if this is correct lmfao
	__remaining->tv_sec  = (time_t)(__time_diff / __frequency.QuadPart);
	__remaining->tv_nsec = time_diff * 1e+9 / __frequency.QuadPart;
	return thrd_success;
}

#else
#error "Have not coded a raw Win32 thread platform yet."
#endif
#endif

typedef struct __ztdc_win32thread_trampoline_t {
	thrd_start_t __func;
	void* __func_arg;
} __ztdc_win32thread_trampoline_t;

inline static DWORD __ztdc_win32thread_trampoline(LPVOID __userdata) {
	ztdc_static_assert(
	     sizeof(DWORD) >= sizeof(int), "size of `int` is too large for a `DWORD`: trampoline will not work");
	thrd_start_t __func = NULL;
	void* __func_arg    = NULL;
	{
		__ztdc_win32thread_trampoline_t* __trampoline_userdata = (__ztdc_win32thread_trampoline_t*)__userdata;
		__func                                                 = __trampoline_userdata->__func;
		__func_arg                                             = __trampoline_userdata->__func_arg;
		free(__trampoline_userdata);
	}
	if (!__func) {
		return 0;
	}
	DWORD __win32_res = 0;
	__win32_res       = __func(__func_arg);
	return __win32_res;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int __ztdc_win32_thrd_create_attrs_err(thrd_t* __thr, thrd_start_t __func, void* __func_arg, size_t __attrs_size,
     const ztdc_thrd_attr_kind** __attrs, ztdc_thrd_attr_err_func_t* __attr_err_func, void* __attr_err_func_arg) {
	const thrd_t __original_thr                   = *__thr;
	const ztdc_thrd_attr_kind* __name_attr_kind   = NULL;
	const ztdc_thrd_attr_kind* __detach_attr_kind = NULL;
	bool __name_set                               = false;
	bool __immediate_detach                       = false;
	wchar_t __name[1024 * 64]; // max size of Win32 thread name
	const size_t __max_name_size = sizeof(__name) - 1;
	ULONG __stack_size           = 1024 * 1000 * 2; // 2 mb default stack
	__ztdc_win32thread_trampoline_t* __trampoline_userdata
	     = (__ztdc_win32thread_trampoline_t*)malloc(sizeof(__ztdc_win32thread_trampoline_t));
	if (__trampoline_userdata == NULL) {
		return thrd_nomem;
	}
	__trampoline_userdata->__func     = __func;
	__trampoline_userdata->__func_arg = __func_arg;

	for (size_t __attr_index = 0; __attr_index < __attrs_size; ++__attr_index) {
		const ztdc_thrd_attr_kind* __attr_kind = __attrs[__attr_index];
		if (__attr_kind == NULL) {
			continue;
		}
		int __attr_err = thrd_success;
		switch (*__attr_kind) {
		case ztdc_thrd_attr_kind_name_sized: {
			ztdc_thrd_attr_name_sized* __attr = (ztdc_thrd_attr_name_sized*)__attr_kind;
			if (__attr->name) {
				const size_t __attr_name_size = __attr->size * sizeof(unsigned char);
				const size_t __copy_size = __attr_name_size > __max_name_size ? __max_name_size : __attr_name_size;
				memcpy(&__name[0], __attr->name, __copy_size);
				__name[__copy_size] = 0;
				__name_set          = true;
				__name_attr_kind    = __attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_mcname: {
			ztdc_thrd_attr_mcname* __attr = (ztdc_thrd_attr_mcname*)__attr_kind;
			if (__attr->name) {
				const char* __attr_name_ptr = __attr->name;
				size_t __attr_name_size     = ztdc_c_string_ptr_size(__attr->name);
				wchar_t* __name_ptr         = __name;
				size_t __name_size          = __max_name_size;
				cnc_mcerr __conv_res
				     = cnc_mcsntomwcsn(&__name_size, &__name_ptr, &__attr_name_size, &__attr_name_ptr);
				if (__conv_res != cnc_mcerr_ok) {
					if (__conv_res == cnc_mcerr_invalid_sequence) {
						__attr_err = thrd_error;
					}
					else {
						__attr_err = thrd_nomem;
					}
				}
				else {
					__name_ptr[0]    = 0;
					__name_set       = true;
					__name_attr_kind = __attr_kind;
				}
			}
		} break;
		case ztdc_thrd_attr_kind_mcname_sized: {
			ztdc_thrd_attr_mcname_sized* __attr = (ztdc_thrd_attr_mcname_sized*)__attr_kind;
			if (__attr->name) {
				const char* __attr_name_ptr = __attr->name;
				size_t __attr_name_size     = __attr->size;
				wchar_t* __name_ptr         = __name;
				size_t __name_size          = __max_name_size;
				cnc_mcerr __conv_res
				     = cnc_mcsntomwcsn(&__name_size, &__name_ptr, &__attr_name_size, &__attr_name_ptr);
				if (__conv_res != cnc_mcerr_ok) {
					if (__conv_res == cnc_mcerr_invalid_sequence) {
						__attr_err = thrd_error;
					}
					else {
						__attr_err = thrd_nomem;
					}
				}
				else {
					__name_ptr[0]    = 0;
					__name_set       = true;
					__name_attr_kind = __attr_kind;
				}
			}
		} break;
		case ztdc_thrd_attr_kind_mwcname_sized: {
			ztdc_thrd_attr_mwcname_sized* __attr = (ztdc_thrd_attr_mwcname_sized*)__attr_kind;
			if (__attr->name) {
				const size_t __attr_name_size = __attr->size * sizeof(__attr->name[0]);
				const size_t __copy_size = __attr_name_size > __max_name_size ? __max_name_size : __attr_name_size;
				memcpy(&__name[0], __attr->name, __copy_size);
				__name[__copy_size] = 0;
				__name_set          = true;
				__name_attr_kind    = __attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_c8name: {
			ztdc_thrd_attr_c8name* __attr = (ztdc_thrd_attr_c8name*)__attr_kind;
			if (__attr->name) {
				const ztd_char8_t* __attr_name_ptr = __attr->name;
				size_t __attr_name_size            = ztdc_c_string_ptr_size(__attr->name);
				wchar_t* __name_ptr                = __name;
				size_t __name_size                 = __max_name_size;
				cnc_mcerr __conv_res
				     = cnc_c8sntomwcsn(&__name_size, &__name_ptr, &__attr_name_size, &__attr_name_ptr);
				if (__conv_res != cnc_mcerr_ok) {
					if (__conv_res == cnc_mcerr_invalid_sequence) {
						__attr_err = thrd_error;
					}
					else {
						__attr_err = thrd_nomem;
					}
				}
				else {
					__name_ptr[0]    = 0;
					__name_set       = true;
					__name_attr_kind = __attr_kind;
				}
			}
		} break;
		case ztdc_thrd_attr_kind_c8name_sized: {
			ztdc_thrd_attr_c8name_sized* __attr = (ztdc_thrd_attr_c8name_sized*)__attr_kind;
			if (__attr->name) {
				const ztd_char8_t* __attr_name_ptr = __attr->name;
				size_t __attr_name_size            = __attr->size;
				wchar_t* __name_ptr                = __name;
				size_t __name_size                 = __max_name_size;
				cnc_mcerr __conv_res
				     = cnc_c8sntomwcsn(&__name_size, &__name_ptr, &__attr_name_size, &__attr_name_ptr);
				if (__conv_res != cnc_mcerr_ok) {
					if (__conv_res == cnc_mcerr_invalid_sequence) {
						__attr_err = thrd_error;
					}
					else {
						__attr_err = thrd_nomem;
					}
				}
				else {
					__name_ptr[0]    = 0;
					__name_set       = true;
					__name_attr_kind = __attr_kind;
				}
			}
		} break;
		case ztdc_thrd_attr_kind_c16name_sized: {
			ztdc_thrd_attr_c16name_sized* __attr = (ztdc_thrd_attr_c16name_sized*)__attr_kind;
			if (__attr->name) {
				const size_t __attr_name_size = __attr->size * sizeof(__attr->name[0]);
				const size_t __copy_size = __attr_name_size > __max_name_size ? __max_name_size : __attr_name_size;
				memcpy(&__name[0], __attr->name, __copy_size);
				__name[__copy_size] = 0;
				__name_set          = true;
				__name_attr_kind    = __attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_c32name: {
			ztdc_thrd_attr_c32name* __attr = (ztdc_thrd_attr_c32name*)__attr_kind;
			if (__attr->name) {
				const ztd_char32_t* __attr_name_ptr = __attr->name;
				size_t __attr_name_size             = ztdc_c_string_ptr_size(__attr->name);
				wchar_t* __name_ptr                 = __name;
				size_t __name_size                  = __max_name_size;
				cnc_mcerr __conv_res
				     = cnc_c32sntomwcsn(&__name_size, &__name_ptr, &__attr_name_size, &__attr_name_ptr);
				if (__conv_res != cnc_mcerr_ok) {
					if (__conv_res == cnc_mcerr_invalid_sequence) {
						__attr_err = thrd_error;
					}
					else {
						__attr_err = thrd_nomem;
					}
				}
				else {
					__name_ptr[0]    = 0;
					__name_set       = true;
					__name_attr_kind = __attr_kind;
				}
			}
		} break;
		case ztdc_thrd_attr_kind_c32name_sized: {
			ztdc_thrd_attr_c32name_sized* __attr = (ztdc_thrd_attr_c32name_sized*)__attr_kind;
			if (__attr->name) {
				const ztd_char32_t* __attr_name_ptr = __attr->name;
				size_t __attr_name_size             = __attr->size;
				wchar_t* __name_ptr                 = __name;
				size_t __name_size                  = __max_name_size;
				cnc_mcerr __conv_res
				     = cnc_c32sntomwcsn(&__name_size, &__name_ptr, &__attr_name_size, &__attr_name_ptr);
				if (__conv_res != cnc_mcerr_ok) {
					if (__conv_res == cnc_mcerr_invalid_sequence) {
						__attr_err = thrd_error;
					}
					else {
						__attr_err = thrd_nomem;
					}
				}
				else {
					__name_ptr[0]    = 0;
					__name_set       = true;
					__name_attr_kind = __attr_kind;
				}
			}
		} break;
		case ztdc_thrd_attr_kind_stack_size: {
			ztdc_thrd_attr_stack_size* __attr = (ztdc_thrd_attr_stack_size*)__attr_kind;
			__stack_size                      = __attr->size;
		} break;
		case ztdc_thrd_attr_kind_detached: {
			ztdc_thrd_attr_detached* __attr = (ztdc_thrd_attr_detached*)__attr_kind;
			__immediate_detach              = __attr->detached;
			__detach_attr_kind              = __attr_kind;
		} break;
		case ztdc_thrd_attr_kind_name:
		case ztdc_thrd_attr_kind_mwcname:
		case ztdc_thrd_attr_kind_c16name: {
			// this is just to recognize the attribute: it's handled below later

		} break;
		default:
			// unrecognized attribute
			__attr_err = thrd_error;
			break;
		}
		if (__attr_err != thrd_success) {
			int __attr_err_res = __attr_err_func(__attr_kind, __attr_err, __attr_err_func_arg);
			if (__attr_err_res != thrd_success) {
				free(__trampoline_userdata);
				return thrd_success;
			}
		}
	}
	// The thread is created suspended so we can set the name on it and other parameters,
	// then we kick it off by setting the name in-line.
	HANDLE* __handle      = __ztdc_win32_handle_ptr(__thr);
	uint32_t* __thread_id = __ztdc_win32_handle_id(__thr);
	DWORD __threadnum;
	*__handle = CreateThread(
	     NULL, __stack_size, __ztdc_win32thread_trampoline, __trampoline_userdata, CREATE_SUSPENDED, &__threadnum);
	if (*__handle == NULL) {
		// TODO: check GetLastError() and translate the failure
		free(__trampoline_userdata);
		return thrd_error;
	}
	*__thread_id = (uint32_t)__threadnum;
	// set the name from the buffer, if we have it
	for (size_t __attr_index = 0; __attr_index < __attrs_size; ++__attr_index) {
		const ztdc_thrd_attr_kind* __attr_kind = __attrs[__attr_index];
		int __attr_err                         = thrd_success;
		switch (*__attr_kind) {
		case ztdc_thrd_attr_kind_name: {
			ztdc_thrd_attr_name* __attr = (ztdc_thrd_attr_name*)__attr_kind;
			if (__attr->name) {
				HRESULT __name_res = SetThreadDescription(*__handle, (PCWSTR)__attr->name);
				if (FAILED(__name_res)) {
					__attr_err = __ztdc_hresult_to_thread_error(__name_res);
				}
				else {
					__name_set = false;
				}
			}
		} break;
		case ztdc_thrd_attr_kind_mwcname: {
			ztdc_thrd_attr_mwcname* __attr = (ztdc_thrd_attr_mwcname*)__attr_kind;
			if (__attr->name) {
				HRESULT __name_res = SetThreadDescription(*__handle, (PCWSTR)__attr->name);
				if (FAILED(__name_res)) {
					__attr_err = __ztdc_hresult_to_thread_error(__name_res);
				}
				else {
					__name_set = false;
				}
			}
		} break;
		case ztdc_thrd_attr_kind_c16name: {
			ztdc_thrd_attr_c16name* __attr = (ztdc_thrd_attr_c16name*)__attr_kind;
			if (__attr->name) {
				HRESULT __name_res = SetThreadDescription(*__handle, (PCWSTR)__attr->name);
				if (FAILED(__name_res)) {
					__attr_err = __ztdc_hresult_to_thread_error(__name_res);
				}
				else {
					__name_set = false;
				}
			}
		} break;
		default:
			// unrecognized attributes already handled above:
			// do nothing here
			break;
		}
		if (__attr_err != thrd_success) {
			int __attr_err_res = __attr_err_func(__attr_kind, __attr_err, __attr_err_func_arg);
			if (__attr_err_res != thrd_success) {
				free(__trampoline_userdata);
				CloseHandle(*__handle);
				*__thr = __original_thr;
				return __attr_err_res;
			}
		}
	}
	if (__name_set) {
		HRESULT __name_res = SetThreadDescription(*__handle, __name);
		if (FAILED(__name_res) && __name_attr_kind) {
			int __attr_err = __ztdc_hresult_to_thread_error(__name_res);
			if (__attr_err != thrd_success) {
				int __attr_err_res = __attr_err_func(__name_attr_kind, __attr_err, __attr_err_func_arg);
				if (__attr_err_res != thrd_success) {
					free(__trampoline_userdata);
					CloseHandle(*__handle);
					*__thr = __original_thr;
					return __attr_err_res;
				}
			}
		}
		else {
			__name_set = false;
		}
	}
	if (__immediate_detach) {
		int __detach_res = thrd_detach(*__thr);
		if (__detach_res != thrd_success && __detach_attr_kind != NULL) {
			int __attr_err_res = __attr_err_func(__detach_attr_kind, __detach_res, __attr_err_func_arg);
			if (__attr_err_res != thrd_success) {
				free(__trampoline_userdata);
				CloseHandle(*__handle);
				*__thr = __original_thr;
				return __attr_err_res;
			}
		}
	}
	// All attributes we care about set: start the thread up now
	DWORD __thread_started = ResumeThread(*__handle);
	if (__thread_started == (DWORD)-1) {
		// well. this is a bit of a problem...
		CloseHandle(*__handle);
		free(__trampoline_userdata);
		*__thr = __original_thr;
		return __ztdc_win32_to_thread_error(GetLastError());
	}
	// 1 means it was suspended, then decremented to zero (what we want)
	// 0 means it was already running (which is... okay?? but also kind of not what we want!!)
	if (__thread_started == (DWORD)0) {
		// if it was already running, than the trampoline userdata might've already been freed?
		// this really should NEVER ever happen.....
		CloseHandle(*__handle);
		*__thr = __original_thr;
		return thrd_error;
	}
	if (__thread_started != (DWORD)1) {
		// somehow, a thread WE created has been hijacked and given a weird, strange value.
		// It's not running, and we've reached the end of this function. Delete
		// everything and call it an error.
		CloseHandle(*__handle);
		free(__trampoline_userdata);
		*__thr = __original_thr;
		return thrd_error;
	}
	return thrd_success;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_mcname(thrd_t __thr, size_t __buffer_size, char* __buffer) {
	if (__buffer == NULL) {
		return thrd_success;
	}
	if (__buffer_size < 2) {
		if (__buffer_size == 0) {
			return thrd_success;
		}
		__buffer[0] = '\0';
		return thrd_success;
	}
	DWORD __id                = *__ztdc_win32_handle_id(&__thr);
	DWORD __current_thread_id = GetCurrentThreadId();
	HANDLE __handle           = NULL;
	if (__id != __current_thread_id) {
		__handle = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, __id);
	}
	else {
		__handle = GetCurrentThread();
	}
	if (__handle == NULL) {
		return thrd_error;
	}
	WCHAR* __impl_wide_str = NULL;
	HRESULT __res          = GetThreadDescription(GetCurrentThread(), &__impl_wide_str);
	if (__id != __current_thread_id) {
		if (CloseHandle(__handle) == 0) {
			if (SUCCEEDED(__res)) {
				LocalFree(__impl_wide_str);
			}
			return thrd_error;
		}
	}
	if (FAILED(__res)) {
		return thrd_error;
	}
	size_t __impl_wide_str_size = ztdc_c_string_ptr_size(__impl_wide_str);
	if (__impl_wide_str_size == 0) {
		LocalFree(__impl_wide_str);
		__buffer[0] = 0;
		return thrd_success;
	}
	WCHAR* __original_impl_wide_str = __impl_wide_str;
	__buffer_size -= 1;
	cnc_mcerr __conv_res
	     = cnc_c16sntomcsn(&__buffer_size, &__buffer, &__impl_wide_str_size, (const ztd_char16_t**)&__impl_wide_str);
	LocalFree(__original_impl_wide_str);
	__buffer[0] = '\0';
	switch (__conv_res) {
	case cnc_mcerr_ok:
		return thrd_success;
	case cnc_mcerr_invalid_sequence:
		return thrd_error;
	default:
		return thrd_nomem;
	}
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_mwcname(thrd_t __thr, size_t __buffer_size, ztd_wchar_t* __buffer) {
	if (__buffer == NULL) {
		return thrd_success;
	}
	if (__buffer_size < 2) {
		if (__buffer_size == 0) {
			return thrd_success;
		}
		__buffer[0] = '\0';
		return thrd_success;
	}
	DWORD __id                = *__ztdc_win32_handle_id(&__thr);
	DWORD __current_thread_id = GetCurrentThreadId();
	HANDLE __handle           = NULL;
	if (__id != __current_thread_id) {
		__handle = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, __id);
	}
	else {
		__handle = GetCurrentThread();
	}
	if (__handle == NULL) {
		return thrd_error;
	}
	WCHAR* __impl_wide_str = NULL;
	HRESULT __res          = GetThreadDescription(GetCurrentThread(), &__impl_wide_str);
	if (__id != __current_thread_id) {
		if (CloseHandle(__handle) == 0) {
			if (SUCCEEDED(__res)) {
				LocalFree(__impl_wide_str);
			}
			return thrd_error;
		}
	}
	if (FAILED(__res)) {
		return thrd_error;
	}
	size_t __impl_wide_str_size = ztdc_c_string_ptr_size(__impl_wide_str);
	if (__impl_wide_str_size == 0) {
		LocalFree(__impl_wide_str);
		__buffer[0] = 0;
		return thrd_success;
	}
	const size_t __impl_wide_str_bytesize = (__impl_wide_str_size) * sizeof(ztd_char16_t);
	const size_t __buffer_bytesize        = (__buffer_size - 1) * sizeof(ztd_char16_t);
	const size_t __copy_size
	     = __buffer_bytesize > __impl_wide_str_bytesize ? __impl_wide_str_bytesize : __buffer_bytesize;
	memcpy(__buffer, __impl_wide_str, __copy_size);
	__buffer[__copy_size / sizeof(wchar_t)] = 0;
	LocalFree(__impl_wide_str);
	return thrd_success;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_c8name(thrd_t __thr, size_t __buffer_size, ztd_char8_t* __buffer) {
	if (__buffer == NULL) {
		return thrd_success;
	}
	if (__buffer_size < 2) {
		if (__buffer_size == 0) {
			return thrd_success;
		}
		__buffer[0] = '\0';
		return thrd_success;
	}
	DWORD __id                = *__ztdc_win32_handle_id(&__thr);
	DWORD __current_thread_id = GetCurrentThreadId();
	HANDLE __handle           = NULL;
	if (__id != __current_thread_id) {
		__handle = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, __id);
	}
	else {
		__handle = GetCurrentThread();
	}
	if (__handle == NULL) {
		return thrd_error;
	}
	WCHAR* __impl_wide_str = NULL;
	HRESULT __res          = GetThreadDescription(GetCurrentThread(), &__impl_wide_str);
	if (__id != __current_thread_id) {
		if (CloseHandle(__handle) == 0) {
			if (SUCCEEDED(__res)) {
				LocalFree(__impl_wide_str);
			}
			return thrd_error;
		}
	}
	if (FAILED(__res)) {
		return thrd_error;
	}
	size_t __impl_wide_str_size = ztdc_c_string_ptr_size(__impl_wide_str);
	if (__impl_wide_str_size == 0) {
		LocalFree(__impl_wide_str);
		__buffer[0] = 0;
		return thrd_success;
	}
	WCHAR* __original_impl_wide_str = __impl_wide_str;
	__buffer_size -= 1;
	cnc_mcerr __conv_res
	     = cnc_c16sntoc8sn(&__buffer_size, &__buffer, &__impl_wide_str_size, (const ztd_char16_t**)&__impl_wide_str);
	LocalFree(__original_impl_wide_str);
	__buffer[0] = '\0';
	switch (__conv_res) {
	case cnc_mcerr_ok:
		return thrd_success;
	case cnc_mcerr_invalid_sequence:
		return thrd_error;
	default:
		return thrd_nomem;
	}
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_c16name(thrd_t __thr, size_t __buffer_size, ztd_char16_t* __buffer) {
	if (__buffer == NULL) {
		return thrd_success;
	}
	if (__buffer_size < 2) {
		if (__buffer_size == 0) {
			return thrd_success;
		}
		__buffer[0] = '\0';
		return thrd_success;
	}
	DWORD __id                = *__ztdc_win32_handle_id(&__thr);
	DWORD __current_thread_id = GetCurrentThreadId();
	HANDLE __handle           = NULL;
	if (__id != __current_thread_id) {
		__handle = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, __id);
	}
	else {
		__handle = GetCurrentThread();
	}
	if (__handle == NULL) {
		return thrd_error;
	}
	WCHAR* __impl_wide_str = NULL;
	HRESULT __res          = GetThreadDescription(__handle, &__impl_wide_str);
	if (__id != __current_thread_id) {
		if (CloseHandle(__handle) == 0) {
			if (SUCCEEDED(__res)) {
				LocalFree(__impl_wide_str);
			}
			return thrd_error;
		}
	}
	if (FAILED(__res)) {
		return thrd_error;
	}
	size_t __impl_wide_str_size = ztdc_c_string_ptr_size(__impl_wide_str);
	if (__impl_wide_str_size == 0) {
		LocalFree(__impl_wide_str);
		__buffer[0] = 0;
		return thrd_success;
	}
	const size_t __impl_wide_str_bytesize = (__impl_wide_str_size) * sizeof(ztd_char16_t);
	const size_t __buffer_bytesize        = (__buffer_size - 1) * sizeof(ztd_char16_t);
	const size_t __copy_size
	     = __buffer_bytesize > __impl_wide_str_bytesize ? __impl_wide_str_bytesize : __buffer_bytesize;
	memcpy(__buffer, __impl_wide_str, __copy_size);
	__buffer[__copy_size / sizeof(ztd_char16_t)] = 0;
	LocalFree(__impl_wide_str);
	return thrd_success;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_c32name(thrd_t __thr, size_t __buffer_size, ztd_char32_t* __buffer) {
	if (__buffer == NULL) {
		return thrd_success;
	}
	if (__buffer_size < 2) {
		if (__buffer_size == 0) {
			return thrd_success;
		}
		__buffer[0] = '\0';
		return thrd_success;
	}
	DWORD __id                = *__ztdc_win32_handle_id(&__thr);
	DWORD __current_thread_id = GetCurrentThreadId();
	HANDLE __handle           = NULL;
	if (__id != __current_thread_id) {
		__handle = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, __id);
	}
	else {
		__handle = GetCurrentThread();
	}
	if (__handle == NULL) {
		return thrd_error;
	}
	WCHAR* __impl_wide_str = NULL;
	HRESULT __res          = GetThreadDescription(GetCurrentThread(), &__impl_wide_str);
	if (__id != __current_thread_id) {
		if (CloseHandle(__handle) == 0) {
			if (SUCCEEDED(__res)) {
				LocalFree(__impl_wide_str);
			}
			return thrd_error;
		}
	}
	if (FAILED(__res)) {
		return thrd_error;
	}
	size_t __impl_wide_str_size = ztdc_c_string_ptr_size(__impl_wide_str);
	if (__impl_wide_str_size == 0) {
		LocalFree(__impl_wide_str);
		__buffer[0] = 0;
		return thrd_success;
	}
	WCHAR* __original_impl_wide_str = __impl_wide_str;
	__buffer_size -= 1;
	cnc_mcerr __conv_res
	     = cnc_c16sntoc32sn(&__buffer_size, &__buffer, &__impl_wide_str_size, (const ztd_char16_t**)&__impl_wide_str);
	__buffer[0] = '\0';
	LocalFree(__original_impl_wide_str);
	switch (__conv_res) {
	case cnc_mcerr_ok:
		return thrd_success;
	case cnc_mcerr_invalid_sequence:
		return thrd_error;
	default:
		return thrd_nomem;
	}
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_mwcname(thrd_t __thr, const ztd_wchar_t* __buffer) {
	if (!__buffer) {
		return thrd_success;
	}
	HRESULT __name_res = SetThreadDescription(*__ztdc_win32_handle_ptr(&__thr), (PCWSTR)__buffer);
	if (FAILED(__name_res)) {
		return __ztdc_hresult_to_thread_error(__name_res);
	}
	return thrd_success;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_c16name(thrd_t __thr, const ztd_char16_t* __buffer) {
	if (!__buffer) {
		return thrd_success;
	}
	HRESULT __name_res = SetThreadDescription(*__ztdc_win32_handle_ptr(&__thr), (PCWSTR)__buffer);
	if (FAILED(__name_res)) {
		return __ztdc_hresult_to_thread_error(__name_res);
	}
	return thrd_success;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_mcname_sized(thrd_t __thr, size_t __buffer_size, const ztd_char_t* __buffer) {
	if (!__buffer) {
		return thrd_success;
	}
	if (__buffer_size < 1u) {
		return thrd_success;
	}
	enum { DESC_MAX_SIZE = 1024 * 64 };
	ztd_wchar_t __tmp_buffer[DESC_MAX_SIZE + 1];
	ztd_wchar_t* __tmp_buffer_ptr = __tmp_buffer;
	size_t __tmp_buffer_size      = DESC_MAX_SIZE;
	cnc_mcerr __conv_err          = cnc_mcsntomwcsn(&__tmp_buffer_size, &__tmp_buffer_ptr, &__buffer_size, &__buffer);
	if (__conv_err != cnc_mcerr_ok) {
		return __conv_err == cnc_mcerr_incomplete_input ? thrd_nomem : thrd_error;
	}
	__tmp_buffer_ptr[0] = 0;
	return ztdc_thrd_set_mwcname(__thr, __tmp_buffer);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_mwcname_sized(thrd_t __thr, size_t __buffer_size, const ztd_wchar_t* __buffer) {
	if (!__buffer) {
		return thrd_success;
	}
	if (__buffer_size < 1u) {
		return thrd_success;
	}
	if (ztdc_c_string_ptr_size_limit_wc(__buffer_size, __buffer) == __buffer_size
	     && __buffer[__buffer_size] == (ztd_wchar_t)0) {
		// it is of the right size, has no intermediate null terminators, and is null-terminated properly
		return ztdc_thrd_set_mwcname(__thr, __buffer);
	}
	enum { DESC_MAX_SIZE = 1024 * 64 };
	if (__buffer_size > DESC_MAX_SIZE) {
		return thrd_nomem;
	}
	// otherwise, we need a temporary buffer
	ztd_wchar_t __tmp_buffer[DESC_MAX_SIZE + 1];
	const size_t __copy_bytesize = __buffer_size * sizeof(ztd_wchar_t);
	memcpy(__tmp_buffer, __buffer, __copy_bytesize);
	__tmp_buffer[__buffer_size] = 0;
	return ztdc_thrd_set_mwcname(__thr, __tmp_buffer);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_c8name_sized(thrd_t __thr, size_t __buffer_size, const ztd_char8_t* __buffer) {
	if (!__buffer) {
		return thrd_success;
	}
	if (__buffer_size < 1u) {
		return thrd_success;
	}
	enum { DESC_MAX_SIZE = 1024 * 64 };
	ztd_wchar_t __tmp_buffer[DESC_MAX_SIZE + 1];
	ztd_wchar_t* __tmp_buffer_ptr = __tmp_buffer;
	size_t __tmp_buffer_size      = DESC_MAX_SIZE;
	cnc_mcerr __conv_err          = cnc_c8sntomwcsn(&__tmp_buffer_size, &__tmp_buffer_ptr, &__buffer_size, &__buffer);
	if (__conv_err != cnc_mcerr_ok) {
		return __conv_err == cnc_mcerr_incomplete_input ? thrd_nomem : thrd_error;
	}
	__tmp_buffer_ptr[0] = 0;
	return ztdc_thrd_set_mwcname(__thr, __tmp_buffer);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_c16name_sized(thrd_t __thr, size_t __buffer_size, const ztd_char16_t* __buffer) {
	if (!__buffer) {
		return thrd_success;
	}
	if (__buffer_size < 1u) {
		return thrd_success;
	}
	if (ztdc_c_string_ptr_size_limit_wc(__buffer_size, __buffer) == __buffer_size
	     && __buffer[__buffer_size] == (ztd_char16_t)0) {
		// it is of the right size, has no intermediate null terminators, and is null-terminated properly
		return ztdc_thrd_set_mwcname(__thr, (const ztd_wchar_t*)__buffer);
	}
	enum { DESC_MAX_SIZE = 1024 * 64 };
	if (__buffer_size > DESC_MAX_SIZE) {
		return thrd_nomem;
	}
	// otherwise, we need a temporary buffer
	ztd_wchar_t __tmp_buffer[DESC_MAX_SIZE + 1];
	const size_t __copy_bytesize = __buffer_size * sizeof(ztd_wchar_t);
	memcpy(__tmp_buffer, __buffer, __copy_bytesize);
	__tmp_buffer[__buffer_size] = 0;
	return ztdc_thrd_set_mwcname(__thr, __tmp_buffer);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_set_c32name_sized(thrd_t __thr, size_t __buffer_size, const ztd_char32_t* __buffer) {
	if (!__buffer) {
		return thrd_success;
	}
	if (__buffer_size < 1u) {
		return thrd_success;
	}
	enum { DESC_MAX_SIZE = 1024 * 64 };
	ztd_wchar_t __tmp_buffer[DESC_MAX_SIZE + 1];
	ztd_wchar_t* __tmp_buffer_ptr = __tmp_buffer;
	size_t __tmp_buffer_size      = DESC_MAX_SIZE;
	cnc_mcerr __conv_err          = cnc_c32sntomwcsn(&__tmp_buffer_size, &__tmp_buffer_ptr, &__buffer_size, &__buffer);
	if (__conv_err != cnc_mcerr_ok) {
		return __conv_err == cnc_mcerr_incomplete_input ? thrd_nomem : thrd_error;
	}
	__tmp_buffer_ptr[0] = 0;
	return ztdc_thrd_set_mwcname(__thr, __tmp_buffer);
}

#endif
