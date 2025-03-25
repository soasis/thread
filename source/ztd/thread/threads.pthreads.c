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

#if ZTD_IS_ON(ZTD_PLATFORM_PTHREADS)

#include <ztd/thread/threads.h>

#include <ztd/idk/static_assert.h>
#include <ztd/idk/null.h>
#include <ztd/idk/size.h>
#include <ztd/idk/detail/threads.pthreads.implementation.h>
#include <ztd/cuneicode.h>

#if ZTD_IS_ON(ZTD_HEADER_SCHED_H)
#include <sched.h>
#endif

#if ZTD_IS_ON(ZTD_CXX)
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdbool>
#include <ctime>
#include <atomic>
#if ZTD_IS_ON(ZTD_HEADER_CSTDATOMIC)
#include <cstdatomic>
#endif
#else
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdatomic.h>
#endif

#include <errno.h>

#if ZTD_IS_ON(ZTD_PLATFORM_AIX) || ZTD_IS_ON(ZTD_PLATFORM_SOLARIS)
#define ZTD_THREAD_THREADS_NAME_SET_INSIDE_I_ ZTD_OFF
#else
#define ZTD_THREAD_THREADS_NAME_SET_INSIDE_I_ ZTD_ON
#endif

#if ZTD_IS_ON(ZTD_THREAD_THREADS_NAME_SET_INSIDE)
#define ZTD_THREAD_THREADS_REQUIRE_SYNCHRONIZATION_I_ ZTD_ON
#else
#define ZTD_THREAD_THREADS_REQUIRE_SYNCHRONIZATION_I_ ZTD_OFF
#endif

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_INTERNAL_LINKAGE)
int __ztdc_pthread_to_thread_error(int __code) {
	switch (__code) {
	case 0:
		return thrd_success;
	case EDEADLK:
	case EBUSY:
		return thrd_busy;
	case EAGAIN:
	case ENOMEM:
	case ERANGE:
		return thrd_nomem;
	case ETIME:
	case ETIMEDOUT:
		return thrd_timedout;
	default:
		return thrd_error;
	}
}


#if ZTD_IS_OFF(ZTD_HEADER_THREADS_H)

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_create(thrd_t* __thr, thrd_start_t __func, void* __func_arg) {
	return ztdc_thrd_create_attrs(__thr, __func, __func_arg, 0, NULL);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_join(thrd_t __thr, int* __res) {
	void* __impl_res;
	int __join_res = pthread_join(__thr, &__impl_res);
	if (__join_res == 0) {
		*__res = (int)(intptr_t)__impl_res;
	}
	return __ztdc_pthread_to_thread_error(__join_res);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
ZTD_USE(ZTD_ATTR_NO_RETURN)
void thrd_exit(int __res) {
	pthread_exit((void*)(intptr_t)(__res));
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_detach(thrd_t __thr) {
	int __res = pthread_detach(__thr);
	return __res == 0 ? thrd_success : thrd_error;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_equal(thrd_t __left, thrd_t __right) {
	return memcmp(&__left, &__right, sizeof(thrd_t)) == 0;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
thrd_t thrd_current(void) {
	return (thrd_t)pthread_self();
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
void thrd_yield(void) {
#if ZTD_IS_ON(ZTD_HEADER_SCHED_H)
	// it's implementation-defiend if it yields anyways:
	// not really our problem if the scehduler/OS doesn't want its time slice back.
	(void)sched_yield();
#elif ZTD_IS_ON(ZTD_PLATFORM_MAC_OS)
	pthread_yield_np();
#elif defined(_GNU_SOURCE) && defined(__USE_GNU)
	// might be deprecated: if this ever builds like this, silence the warning
	// it's implementation-defiend if it yields anyways:
	// not really our problem if the scehduler/OS doesn't want its time slice back.
	(void)pthread_yield();
#else
#error "unknown platform"
#endif
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int thrd_sleep(const struct timespec* __duration, struct timespec* __remaining) {
	// this is the only funtion where
	return nanosleep(__duration, __remaining);
}

#endif

typedef enum __ztdc_encoding_name {
	__ztdc_encoding_name_none,
	__ztdc_encoding_name_mc,
	__ztdc_encoding_name_mwc,
	__ztdc_encoding_name_c8,
	__ztdc_encoding_name_c16,
	__ztdc_encoding_name_c32,
} __ztdc_encoding_name;

typedef struct __ztdc_pthread_trampoline_t {
	thrd_start_t __func;
	void* __func_arg;
#if ZTD_IS_ON(ZTD_THREAD_THREADS_REQUIRE_SYNCHRONIZATION)
	atomic_bool* __sync_until;
	atomic_bool* __sync_still_ok;
	int* __sync_result;
	ztdc_thrd_attr_kind* __sync_kind;
#if ZTD_IS_ON(ZTD_THREAD_THREADS_NAME_SET_INSIDE)
	char __name[ZTD_USE(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)];
	bool __name_set;
#endif
#endif
} __ztdc_pthread_trampoline_t;

inline static int __ztdc_pthread_prepare_name_trampoline(pthread_attr_t* __impl_attrs,
     __ztdc_pthread_trampoline_t* __trampoline_userdata, size_t __name_size, const void* __name,
     __ztdc_encoding_name __name_encoding) {
	int __err = thrd_success;
#if ZTD_IS_ON(ZTD_THREAD_THREADS_NAME_SET_INSIDE)
	(void)__impl_attrs;
	switch (__name_encoding) {
	case __ztdc_encoding_name_c32: {
		const ztd_char32_t* __name_ptr = (const ztd_char32_t*)__name;
		char* __trampoline_name_ptr    = __trampoline_userdata->__name;
		size_t __trampoline_name_size  = sizeof(__trampoline_userdata->__name) - 1;
		cnc_mcerr __conv_res
		     = cnc_c32sntomcsn_utf8(&__trampoline_name_size, &__trampoline_name_ptr, &__name_size, &__name_ptr);
		if (__conv_res == cnc_mcerr_ok) {
			__trampoline_name_ptr[0] = '\0';
		}
		else {
			__err = thrd_error;
		}
	} break;
	case __ztdc_encoding_name_c16: {
		const ztd_char16_t* __name_ptr = (const ztd_char16_t*)__name;
		char* __trampoline_name_ptr    = __trampoline_userdata->__name;
		size_t __trampoline_name_size  = sizeof(__trampoline_userdata->__name) - 1;
		cnc_mcerr __conv_res
		     = cnc_c16sntomcsn_utf8(&__trampoline_name_size, &__trampoline_name_ptr, &__name_size, &__name_ptr);
		if (__conv_res == cnc_mcerr_ok) {
			__trampoline_name_ptr[0] = '\0';
		}
		else {
			__err = thrd_error;
		}
	} break;
	case __ztdc_encoding_name_mc: {
		const ztd_char_t* __name_ptr  = (const ztd_char_t*)__name;
		char* __trampoline_name_ptr   = __trampoline_userdata->__name;
		size_t __trampoline_name_size = sizeof(__trampoline_userdata->__name) - 1;
		cnc_mcerr __conv_res
		     = cnc_mcsntomcsn_exec_utf8(&__trampoline_name_size, &__trampoline_name_ptr, &__name_size, &__name_ptr);
		if (__conv_res == cnc_mcerr_ok) {
			__trampoline_name_ptr[0] = '\0';
		}
		else {
			__err = thrd_error;
		}
	} break;
	case __ztdc_encoding_name_mwc: {
		const ztd_wchar_t* __name_ptr = (const ztd_wchar_t*)__name;
		char* __trampoline_name_ptr   = __trampoline_userdata->__name;
		size_t __trampoline_name_size = sizeof(__trampoline_userdata->__name) - 1;
		cnc_mcerr __conv_res          = cnc_mwcsntomcsn_wide_exec_utf8(
               &__trampoline_name_size, &__trampoline_name_ptr, &__name_size, &__name_ptr);
		if (__conv_res == cnc_mcerr_ok) {
			__trampoline_name_ptr[0] = '\0';
		}
		else {
			__err = thrd_error;
		}
	} break;
	case __ztdc_encoding_name_c8:
	case __ztdc_encoding_name_none:
	default: {
		const size_t __trampoline_name_size = sizeof(__trampoline_userdata->__name) - 1;
		const size_t __copy_size = __trampoline_name_size < __name_size ? __trampoline_name_size : __name_size;
		memcpy(__trampoline_userdata->__name, __name, __copy_size);
		__trampoline_userdata->__name[__copy_size] = '\0'; // ensure the necessary null termination
	} break;
	}
#else
	// we're gonna be using the IBM stuff here, then
	switch (__name_encoding) {
	case __ztdc_encoding_name_c32: {
		char __pivot[ZTD_USE(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)];
		char* __pivot_ptr   = __pivot;
		size_t __pivot_size = sizeof(__pivot) - 1;
		cnc_mcerr __conv_res
		     = cnc_c32sntomcsn_utf8(&__pivot_size, &__pivot_ptr, __name_size, (const ztd_char32_t**)&__name);
		if (__conv_res == cnc_mcerr_ok) {
			__pivot_ptr[0] = '\0';
		}
		else {
			__err = thrd_error;
			break;
		}
		int __name_err = pthread_attr_setname_np(__impl_attrs, __pivot);
		if (__name_err != 0) {
			__err = __ztdc_pthread_to_thread_error(__name_err);
		}
	} break;
	case __ztdc_encoding_name_c16: {
		char __pivot[ZTD_USE(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)];
		char* __pivot_ptr   = __pivot;
		size_t __pivot_size = sizeof(__pivot) - 1;
		cnc_mcerr __conv_res
		     = cnc_c16sntomcsn_utf8(&__pivot_size, &__pivot_ptr, __name_size, (const ztd_char16_t**)&__name);
		if (__conv_res == cnc_mcerr_ok) {
			__pivot_ptr[0] = '\0';
		}
		else {
			__err = thrd_error;
			break;
		}
		int __name_err = pthread_attr_setname_np(__impl_attrs, __pivot);
		if (__name_err != 0) {
			__err = __ztdc_pthread_to_thread_error(__name_err);
		}
	} break;
	case __ztdc_encoding_name_mc: {
		char __pivot[ZTD_USE(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)];
		char* __pivot_ptr   = __pivot;
		size_t __pivot_size = sizeof(__pivot) - 1;
		cnc_mcerr __conv_res
		     = cnc_mcsntomcsn_exec_utf8(&__pivot_size, &__pivot_ptr, __name_size, (const ztd_char_t**)&__name);
		if (__conv_res == cnc_mcerr_ok) {
			__pivot_ptr[0] = '\0';
		}
		else {
			__err = thrd_error;
			break;
		}
		int __name_err = pthread_attr_setname_np(__impl_attrs, __pivot);
		if (__name_err != 0) {
			__err = __ztdc_pthread_to_thread_error(__name_err);
		}
	} break;
	case __ztdc_encoding_name_mwc: {
		char __pivot[ZTD_USE(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)];
		char* __pivot_ptr    = __pivot;
		size_t __pivot_size  = sizeof(__pivot) - 1;
		cnc_mcerr __conv_res = cnc_mwcsntomcsn_wide_exec_utf8(
		     &__pivot_size, &__pivot_ptr, __name_size, (const ztd_wchar_t_t**)&__name);
		if (__conv_res == cnc_mcerr_ok) {
			__pivot_ptr[0] = '\0';
		}
		else {
			__err = thrd_error;
			break;
		}
		int __name_err = pthread_attr_setname_np(__impl_attrs, __pivot);
		if (__name_err != 0) {
			__err = __ztdc_pthread_to_thread_error(__name_err);
		}
	} break;
	case __ztdc_encoding_name_c8:
	case __ztdc_encoding_name_none:
	default: {
		int __name_err = pthread_attr_setname_np(__impl_attrs, (const char*)__name);
		if (__name_err != 0) {
			__err = __ztdc_pthread_to_thread_error(__name_err);
		}
	} break;
	}
#endif
	if (__err == thrd_success) {
		__trampoline_userdata->__name_set = true;
	}
	return __err;
}

inline static void* __ztdc_pthread_trampoline(void* __userdata) {
	ztdc_static_assert(
	     sizeof(void*) >= sizeof(int), "size of `int` is too large for a `void*`: trampoline will not work");
	thrd_start_t __func              = NULL;
	void* __func_arg                 = NULL;
	int* __sync_result               = NULL;
	atomic_bool* __sync_until        = NULL;
	atomic_bool* __sync_still_ok     = NULL;
	ztdc_thrd_attr_kind* __sync_kind = NULL;
#if ZTD_IS_ON(ZTD_THREAD_THREADS_NAME_SET_INSIDE)
	int __pre_err = thrd_success;
#endif
	{
		__ztdc_pthread_trampoline_t* __trampoline_userdata = (__ztdc_pthread_trampoline_t*)__userdata;
		__func                                             = __trampoline_userdata->__func;
		__func_arg                                         = __trampoline_userdata->__func_arg;
#if ZTD_IS_ON(ZTD_THREAD_THREADS_REQUIRE_SYNCHRONIZATION)
		__sync_result   = __trampoline_userdata->__sync_result;
		__sync_until    = __trampoline_userdata->__sync_until;
		__sync_still_ok = __trampoline_userdata->__sync_still_ok;
		__sync_kind     = __trampoline_userdata->__sync_kind;
#if ZTD_IS_ON(ZTD_THREAD_THREADS_NAME_SET_INSIDE)
		if (__trampoline_userdata->__name_set) {
#if ZTD_IS_ON(ZTD_PLATFORM_MAC_OS)
			// The setname uses the "current thread" on MacOS, so it has to be set here
			__pre_err = __ztdc_pthread_to_thread_error(pthread_setname_np(__trampoline_userdata->__name));
#else
			// In other APIs, you have to produce the thread first.
			// In some parts of pthread you can create a thread but in the "stopped" mode;
			// you can "start" it up against later. But this isn't portable. Unfortunately.
			// So we set it in here, too.
			// Only on IBM-ish systems can you set the name ahead-of-time with `attr_setname_np`
			// also I am too lazy to create a thread in "stopped" mode, then set up the name while I
			// still have thrd_t, and then start it up once it's set. It would save on the malloc(...) for
			// the __name array...
			pthread_t __self_thread = pthread_self();
#if ZTD_IS_ON(ZTD_PLATFORM_NETBSD)
			__pre_err = __ztdc_pthread_to_thread_error(
			     pthread_setname_np(__self_thread, __trampoline_userdata->__name, 0)); // name + void* arg -- huh??
#elif ZTD_IS_ON(ZTD_PLATFORM_FREEBSD) || ZTD_IS_ON(ZTD_PLATFORM_OPENBSD)
			// same as most other *nix but different spelling
			pthread_set_name_np(__self_thread, __trampoline_userdata->__name);
#else
			// everyone else is mildly normal about this.
			__pre_err
			     = __ztdc_pthread_to_thread_error(pthread_setname_np(__self_thread, __trampoline_userdata->__name));
#endif
#endif
		}
#endif
		free(__trampoline_userdata);
#endif
	}
	void* __pthread_res = 0;
	memset(&__pthread_res, 0, sizeof(void*)); // nullptr rep does not have to be "all bits zero" on all platforms, lol.
#if ZTD_IS_ON(ZTD_THREAD_THREADS_REQUIRE_SYNCHRONIZATION)
	if (__pre_err != thrd_success) {
		*__sync_result = __pre_err;
		atomic_store(__sync_until, false);
		while (atomic_load(__sync_still_ok)) {
			// wait for feedback for this error.
		}
		if (*__sync_result != thrd_success) {
			return __pthread_res;
		}
	}
#endif
#if ZTD_IS_ON(ZTD_THREAD_THREADS_REQUIRE_SYNCHRONIZATION)
	atomic_store(__sync_until, false);
#endif
	if (!__func) {
		// this should never happen, but we're just checking
		return __pthread_res;
	}
	int __std_res = __func(__func_arg);
	memcpy(__pthread_res, &__std_res, sizeof(__std_res));
	return __pthread_res;
}

inline static int __ztdc_ignore_all_thrd_errors(ztdc_thrd_attr_kind __kind, int __err, void* __userdata) {
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
     const ztdc_thrd_attr_kind** __attrs, ztdc_thrd_attr_err_func_t* __attr_err_func, void* __attr_err_func_userdata) {
#if ZTD_IS_ON(ZTD_HEADER_THREADS_H)
	ztdc_static_assert(sizeof(thrd_t) >= sizeof(pthread_t), "thrd_t is not the same size as pthread_t");
#endif
	if (!__func) {
		return thrd_success;
	}
	int __sync_result = thrd_success;
	__ztdc_pthread_trampoline_t* __trampoline_userdata
	     = (__ztdc_pthread_trampoline_t*)malloc(sizeof(__ztdc_pthread_trampoline_t));
	__trampoline_userdata->__func     = __func;
	__trampoline_userdata->__func_arg = __func_arg;
#if ZTD_IS_ON(ZTD_THREAD_THREADS_REQUIRE_SYNCHRONIZATION)
	ztdc_thrd_attr_kind __sync_kind        = ztdc_thrd_attr_kind_impl_def;
	atomic_bool __sync_until               = true;
	atomic_bool __sync_still_ok            = true;
	__trampoline_userdata->__sync_until    = &__sync_until;
	__trampoline_userdata->__sync_still_ok = &__sync_still_ok;
	__trampoline_userdata->__sync_result   = &__sync_result;
	__trampoline_userdata->__sync_kind     = &__sync_kind;
#endif
	pthread_attr_t __impl_attrs = { 0 };
	int __impl_attr_init_res    = pthread_attr_init(&__impl_attrs);
	if (__impl_attr_init_res != 0) {
		return __ztdc_pthread_to_thread_error(__impl_attr_init_res);
	}
	for (size_t __attr_index = 0; __attr_index < __attrs_size; ++__attr_index) {
		const ztdc_thrd_attr_kind* __attr_kind = __attrs[__attr_index];
		int __attr_err                         = thrd_success;
		switch (*__attr_kind) {
		case ztdc_thrd_attr_kind_name: {
			ztdc_thrd_attr_name* __attr = (ztdc_thrd_attr_name*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = ztdc_c_string_ptr_size((const char*)__attr->name);
				__attr_err  = __ztdc_pthread_prepare_name_trampoline(&__impl_attrs, __trampoline_userdata,
				      __name_size, (const char*)__attr->name, __ztdc_encoding_name_none);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_name_sized: {
			ztdc_thrd_attr_name_sized* __attr = (ztdc_thrd_attr_name_sized*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = __attr->size;
				__attr_err  = __ztdc_pthread_prepare_name_trampoline(&__impl_attrs, __trampoline_userdata,
				      __name_size, (const char*)__attr->name, __ztdc_encoding_name_none);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_mcname: {
			ztdc_thrd_attr_mcname* __attr = (ztdc_thrd_attr_mcname*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = ztdc_c_string_ptr_size(__attr->name);
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_mc);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_mcname_sized: {
			ztdc_thrd_attr_mcname_sized* __attr = (ztdc_thrd_attr_mcname_sized*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = __attr->size;
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_mc);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_mwcname: {
			ztdc_thrd_attr_mwcname* __attr = (ztdc_thrd_attr_mwcname*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = ztdc_c_string_ptr_size(__attr->name);
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_mwc);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_mwcname_sized: {
			ztdc_thrd_attr_mwcname_sized* __attr = (ztdc_thrd_attr_mwcname_sized*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = __attr->size;
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_mwc);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_c8name: {
			ztdc_thrd_attr_c8name* __attr = (ztdc_thrd_attr_c8name*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = ztdc_c_string_ptr_size(__attr->name);
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_c8);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_c8name_sized: {
			ztdc_thrd_attr_c8name_sized* __attr = (ztdc_thrd_attr_c8name_sized*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = __attr->size;
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_c8);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_c16name: {
			ztdc_thrd_attr_c16name* __attr = (ztdc_thrd_attr_c16name*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = ztdc_c_string_ptr_size(__attr->name);
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_c16);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_c16name_sized: {
			ztdc_thrd_attr_c16name_sized* __attr = (ztdc_thrd_attr_c16name_sized*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = __attr->size;
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_c16);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_c32name: {
			ztdc_thrd_attr_c32name* __attr = (ztdc_thrd_attr_c32name*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = ztdc_c_string_ptr_size(__attr->name);
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_c32);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_c32name_sized: {
			ztdc_thrd_attr_c32name_sized* __attr = (ztdc_thrd_attr_c32name_sized*)__attr_kind;
			if (__attr->name) {
				const size_t __name_size = __attr->size;
				__attr_err               = __ztdc_pthread_prepare_name_trampoline(
                         &__impl_attrs, __trampoline_userdata, __name_size, __attr->name, __ztdc_encoding_name_c32);
				__sync_kind = *__attr_kind;
			}
		} break;
		case ztdc_thrd_attr_kind_stack_size: {
			ztdc_thrd_attr_stack_size* __attr = (ztdc_thrd_attr_stack_size*)__attr_kind;
			__attr_err = __ztdc_pthread_to_thread_error(pthread_attr_setstacksize(&__impl_attrs, __attr->size));
		} break;
		case ztdc_thrd_attr_kind_detached: {
			ztdc_thrd_attr_detached* __attr = (ztdc_thrd_attr_detached*)__attr_kind;
			__attr_err                      = __ztdc_pthread_to_thread_error(pthread_attr_setdetachstate(
                    &__impl_attrs, __attr->detached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE));
		} break;
		case ztdc_thrd_attr_kind__stack_storage: {
			ztdc_thrd_attr_stack_storage* __attr = (ztdc_thrd_attr_stack_storage*)__attr_kind;
			__attr_err                           = __ztdc_pthread_to_thread_error(
                    pthread_attr_setstack(&__impl_attrs, __attr->buffer, __attr->size));
		} break;
		case ztdc_thrd_attr_kind__stack_guard_size: {
			ztdc_thrd_attr_stack_guard_size* __attr = (ztdc_thrd_attr_stack_guard_size*)__attr_kind;
			__attr_err = __ztdc_pthread_to_thread_error(pthread_attr_setguardsize(&__impl_attrs, __attr->size));
		} break;
		default:
			// unhandled attributes are considered errors
			__attr_err = thrd_error;
			break;
		}
		if (__attr_err != thrd_success) {
			// process errors through the error function
			int __attr_err_res = __attr_err_func(*__attr_kind, __attr_err, __attr_err_func_userdata);
			if (__attr_err_res != thrd_success) {
				int __impl_attr_destroy_res = pthread_attr_destroy(&__impl_attrs);
				(void)__impl_attr_destroy_res;
				return __attr_err_res;
			}
		}
	}
#if ZTD_IS_ON(ZTD_HEADER_THREADS_H)
	pthread_t __raw_thr = { 0 };
	int __result        = pthread_create(&__raw_thr, &__impl_attrs, __ztdc_pthread_trampoline, __trampoline_userdata);
	if (__result == 0) {
		*__thr = (thrd_t)__raw_thr;
	}
#else
	int __result = pthread_create(__thr, &__impl_attrs, __ztdc_pthread_trampoline, __trampoline_userdata);
#endif
	int __attr_res = pthread_attr_destroy(&__impl_attrs);
	(void)__attr_res;
	if (__result != 0) {
		return __ztdc_pthread_to_thread_error(__result);
	}
#if ZTD_IS_ON(ZTD_THREAD_THREADS_REQUIRE_SYNCHRONIZATION)
	while (atomic_load(&__sync_until)) {
		// spinlock until the thread is started and all internal work is okay.
	}
	// return any from-thread failures and bail if necessary
	int __post_thread_start_results = __sync_result;
	if (__post_thread_start_results != thrd_success) {
		__post_thread_start_results
		     = __attr_err_func(__sync_kind, __post_thread_start_results, __attr_err_func_userdata);
		__sync_result = __post_thread_start_results;
		// send back the result to the thread so it knows to either go or quit
		atomic_store(&__sync_still_ok, false);
		if (__post_thread_start_results != thrd_success) {
			return __post_thread_start_results;
		}
	}
	// all is done, let the thread go ahead with the correct data.
	atomic_store(&__sync_still_ok, false);
#endif
	return thrd_success;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_name(thrd_t __thr, size_t __buffer_size, void* __buffer) {
	int __res = pthread_getname_np((pthread_t)__thr, (char*)__buffer, __buffer_size);
	return __ztdc_pthread_to_thread_error(__res);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_mcname(thrd_t __thr, size_t __buffer_size, char* __buffer) {
	ztd_char8_t __pivot[ZTD_USE(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)];
	const size_t __pivot_size      = ztdc_c_array_size(__pivot);
	const ztd_char8_t* __pivot_ptr = (const ztd_char8_t*)&__pivot[0];
	int __res                      = pthread_getname_np((pthread_t)__thr, (char*)__pivot, __pivot_size);
	if (__res != 0) {
		return __ztdc_pthread_to_thread_error(__res);
	}
	size_t __real_pivot_size = ztdc_c_string_ptr_size_limit(__pivot_size, __pivot);
	cnc_mcerr __conv_res     = cnc_c8sntomcsn(&__buffer_size, &__buffer, &__real_pivot_size, &__pivot_ptr);
	if (__conv_res == cnc_mcerr_ok) {
		return thrd_success;
	}
	if (__conv_res == cnc_mcerr_invalid_sequence) {
		return thrd_nomem;
	}
	return thrd_error;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_mwcname(thrd_t __thr, size_t __buffer_size, ztd_wchar_t* __buffer) {
	ztd_char8_t __pivot[ZTD_USE(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)];
	const size_t __pivot_size      = ztdc_c_array_size(__pivot);
	const ztd_char8_t* __pivot_ptr = (const ztd_char8_t*)&__pivot[0];
	int __res                      = pthread_getname_np((pthread_t)__thr, (char*)__pivot, __pivot_size);
	if (__res != 0) {
		return __ztdc_pthread_to_thread_error(__res);
	}
	size_t __real_pivot_size = ztdc_c_string_ptr_size_limit(__pivot_size, __pivot);
	cnc_mcerr __conv_res     = cnc_c8sntomwcsn(&__buffer_size, &__buffer, &__real_pivot_size, &__pivot_ptr);
	if (__conv_res == cnc_mcerr_ok) {
		return thrd_success;
	}
	if (__conv_res == cnc_mcerr_invalid_sequence) {
		return thrd_nomem;
	}
	return thrd_error;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_c8name(thrd_t __thr, size_t __buffer_size, ztd_char8_t* __buffer) {
	int __res = pthread_getname_np((pthread_t)__thr, (char*)__buffer, __buffer_size);
	return __ztdc_pthread_to_thread_error(__res);
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_c16name(thrd_t __thr, size_t __buffer_size, ztd_char16_t* __buffer) {
	ztd_char8_t __pivot[ZTD_USE(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)];
	const size_t __pivot_size      = ztdc_c_array_size(__pivot);
	const ztd_char8_t* __pivot_ptr = (const ztd_char8_t*)&__pivot[0];
	int __res                      = pthread_getname_np((pthread_t)__thr, (char*)__pivot, __pivot_size);
	if (__res != 0) {
		return __ztdc_pthread_to_thread_error(__res);
	}
	size_t __real_pivot_size = ztdc_c_string_ptr_size_limit(__pivot_size, __pivot);
	cnc_mcerr __conv_res     = cnc_c8sntoc16sn(&__buffer_size, &__buffer, &__real_pivot_size, &__pivot_ptr);
	if (__conv_res == cnc_mcerr_ok) {
		return thrd_success;
	}
	if (__conv_res == cnc_mcerr_invalid_sequence) {
		return thrd_nomem;
	}
	return thrd_error;
}

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_LINKAGE)
int ztdc_thrd_get_c32name(thrd_t __thr, size_t __buffer_size, ztd_char32_t* __buffer) {
	ztd_char8_t __pivot[ZTD_USE(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)];
	const size_t __pivot_size      = ztdc_c_array_size(__pivot);
	const ztd_char8_t* __pivot_ptr = (const ztd_char8_t*)&__pivot[0];
	int __res                      = pthread_getname_np((pthread_t)__thr, (char*)__pivot, __pivot_size);
	if (__res != 0) {
		return __ztdc_pthread_to_thread_error(__res);
	}
	size_t __real_pivot_size = ztdc_c_string_ptr_size_limit(__pivot_size, __pivot);
	cnc_mcerr __conv_res     = cnc_c8sntoc32sn(&__buffer_size, &__buffer, &__real_pivot_size, &__pivot_ptr);
	if (__conv_res == cnc_mcerr_ok) {
		return thrd_success;
	}
	if (__conv_res == cnc_mcerr_invalid_sequence) {
		return thrd_error;
	}
	return thrd_nomem;
}

#endif
