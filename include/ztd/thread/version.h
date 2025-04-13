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

#ifndef ZTD_THREAD_VERSION_H
#define ZTD_THREAD_VERSION_H

#include <ztd/idk/version.h>

#include <ztd/thread/detail/api.h>

#define ZTD_THREAD_VERSION_MAJOR 0
#define ZTD_THREAD_VERSION_MINOR 5
#define ZTD_THREAD_VERSION_PATCH 0
#define ZTD_THREAD_VERSION_STRING \
	ZTD_MAKE_VERSION_STR(ZTD_THREAD_VERSION_MAJOR, ZTD_THREAD_VERSION_MINOR, ZTD_THREAD_VERSION_PATCH)
#define ZTD_THREAD_VERSION \
	ZTD_MAKE_VERSION_INT(ZTD_THREAD_VERSION_MAJOR, ZTD_THREAD_VERSION_MINOR, ZTD_THREAD_VERSION_PATCH)

// clang-format off
#if defined(ZTD_THREAD_PTHREAD_BASED)
	#if (ZTD_THREAD_PTHREAD_BASED) != 0
		#define ZTD_THREAD_PTHREAD_BASED_I_ ZTD_ON
	#else
		#define ZTD_THREAD_PTHREAD_BASED_I_ ZTD_OFF
	#endif
#elif ZTD_IS_ON(ZTD_HEADER_PTHREAD_H)
	#define ZTD_THREAD_PTHREAD_BASED_I_ ZTD_DEFAULT_ON
#else
	#define ZTD_THREAD_PTHREAD_BASED_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_THREAD_WIN32_XTHREAD_BASED)
	#if (ZTD_THREAD_WIN32_XTHREAD_BASED) != 0
		#define ZTD_THREAD_WIN32_XTHREAD_BASED_I_ ZTD_ON
	#else
		#define ZTD_THREAD_WIN32_XTHREAD_BASED_I_ ZTD_OFF
	#endif
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS) && ZTD_IS_ON(ZTD_HEADER_XTHREADS_H)
	#define ZTD_THREAD_WIN32_XTHREAD_BASED_I_ ZTD_DEFAULT_ON
#else
	#define ZTD_THREAD_WIN32_XTHREAD_BASED_I_ ZTD_DEFAULT_OFF
#endif

#if defined(ZTD_THREAD_WIN32_BASED)
	#if (ZTD_THREAD_WIN32_BASED) != 0
		#define ZTD_THREAD_WIN32_BASED_I_ ZTD_ON
	#else
		#define ZTD_THREAD_WIN32_BASED_I_ ZTD_OFF
	#endif
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS) && ZTD_IS_OFF(ZTD_THREAD_PTHREAD_BASED)
	#define ZTD_THREAD_WIN32_BASED_I_ ZTD_DEFAULT_ON
#else
	#define ZTD_THREAD_WIN32_BASED_I_ ZTD_DEFAULT_OFF
#endif

#if ZTD_IS_ON(ZTD_THREAD_WIN32_XTHREAD_BASED) || ZTD_IS_ON(ZTD_THREAD_WIN32_BASED)
	#define ZTD_THREAD_ANY_WIN32_BASED_I_ ZTD_DEFAULT_ON
#else
	#define ZTD_THREAD_ANY_WIN32_BASED_I_ ZTD_DEFAULT_OFF
#endif



#if defined(ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE)
	#define ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE_I_ ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE
#else
	#if ZTD_IS_ON(ZTD_COMPILER_VCXX) || ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
		// "The reserve value specifies the total stack allocation in virtual memory. 
		// For ARM, x86 and x64 machines, the default stack size is 1 MB."
		// ...
		// "For ARM, x86 and x64 machines, the default commit value is 4 KB"
		// https://docs.microsoft.com/en-us/cpp/build/reference/stack-stack-allocations?view=vs-2019
		// Uses: (1024 * 64)
		#define ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE_I_ (65536)
	#elif ZTD_IS_ON(ZTD_PLATFORM_MAC_OS)
		// "  -stack_size size
		//     Specifies the maximum stack size for the main thread in a program.  Without this option a
		//     program has a 8MB stack.  The argument size is a hexadecimal number with an optional lead-
		//     ing 0x. The size should be a multiple of the architecture's page size (4KB or 16KB).
		// ld(1) manpage on Mac OS
		// Uses: 1024 * 4
		#define ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE_I_ (4096)
	#elif ZTD_IS_ON(ZTD_PLATFORM_LINUX) || ZTD_IS_ON(ZTD_PLATFORM_UNIX)
		// "Here is the vale for a few architectures:"
		//
		//    │Architecture │ Default stack size │
		//    ├─────────────┼────────────────────┤
		//    │i386         │               2 MB │
		//    ├─────────────┼────────────────────┤
		//    │IA-64        │              32 MB │
		//    ├─────────────┼────────────────────┤
		//    │PowerPC      │               4 MB │
		//    ├─────────────┼────────────────────┤
		//    │S/390        │               2 MB │
		//    ├─────────────┼────────────────────┤
		//    │Sparc-32     │               2 MB │
		//    ├─────────────┼────────────────────┤
		//    │Sparc-64     │               4 MB │
		//    ├─────────────┼────────────────────┤
		//    │x86_64       │               2 MB │
		//    └─────────────┴────────────────────┘
		// http://man7.org/linux/man-pages/man3/pthread_create.3.html
		// Uses: 1024 * 2
		#define ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE_I_ (2048)
	#else
		// Tiny embbeded compiler shenanigans??
		// Uses: 1024 / 8
		#define ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE_I_ (128)
	#endif // MSVC vs. others
#endif // Intermediate buffer sizing

#define ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_SIZE(...) (ZTD_THREAD_INTERMEDIATE_BUFFER_SUGGESTED_BYTE_SIZE_I_ / sizeof(__VA_ARGS__))

// clang-format on

#endif
