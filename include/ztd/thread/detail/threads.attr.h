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

#ifndef ZTD_THREAD_DETAIL_THREADS_ATTR_H
#define ZTD_THREAD_DETAIL_THREADS_ATTR_H

#include <ztd/thread/version.h>

#include <ztd/idk/charN_t.h>

#if ZTD_IS_ON(ZTD_CXX)
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

typedef enum ztdc_thrd_attr_kind
#if ZTD_IS_ON(ZTD_CXX) || (ZTD_IS_ON(ZTD_C) && __STDC_VERSION__ >= 202300L)
: int_least32_t
#endif
{ ztdc_thrd_attr_kind_name                 = 0,
	ztdc_thrd_attr_kind_name_sized        = 1,
	ztdc_thrd_attr_kind_mcname            = 2,
	ztdc_thrd_attr_kind_mcname_sized      = 3,
	ztdc_thrd_attr_kind_mwcname           = 4,
	ztdc_thrd_attr_kind_mwcname_sized     = 5,
	ztdc_thrd_attr_kind_c8name            = 6,
	ztdc_thrd_attr_kind_c8name_sized      = 7,
	ztdc_thrd_attr_kind_c16name           = 8,
	ztdc_thrd_attr_kind_c16name_sized     = 9,
	ztdc_thrd_attr_kind_c32name           = 10,
	ztdc_thrd_attr_kind_c32name_sized     = 11,
	ztdc_thrd_attr_kind_stack_size        = 32,
	ztdc_thrd_attr_kind_detached          = 256,
	ztdc_thrd_attr_kind_impl_def          = 0xFFFF,
	ztdc_thrd_attr_kind__stack_storage    = 0x10020,
	ztdc_thrd_attr_kind__stack_guard_size = 0x10021,
} ztdc_thrd_attr_kind;

// STANDARD THREAD ATTRIBUTES
typedef struct ztdc_thrd_attr_name {
	ztdc_thrd_attr_kind kind;
	const void* name;
} ztdc_thrd_attr_name;

typedef struct ztdc_thrd_attr_name_sized {
	ztdc_thrd_attr_kind kind;
	size_t size;
	const void* name;
} ztdc_thrd_attr_name_sized;

typedef struct ztdc_thrd_attr_mcname {
	ztdc_thrd_attr_kind kind;
	const char* name;
} ztdc_thrd_attr_mcname;

typedef struct ztdc_thrd_attr_mcname_sized {
	ztdc_thrd_attr_kind kind;
	size_t size;
	const char* name;
} ztdc_thrd_attr_mcname_sized;

typedef struct ztdc_thrd_attr_mwcname {
	ztdc_thrd_attr_kind kind;
	const ztd_wchar_t* name;
} ztdc_thrd_attr_mwcname;

typedef struct ztdc_thrd_attr_mwcname_sized {
	ztdc_thrd_attr_kind kind;
	size_t size;
	const ztd_wchar_t* name;
} ztdc_thrd_attr_mwcname_sized;

typedef struct ztdc_thrd_attr_c8name {
	ztdc_thrd_attr_kind kind;
	const ztd_char8_t* name;
} ztdc_thrd_attr_c8name;

typedef struct ztdc_thrd_attr_c8name_sized {
	ztdc_thrd_attr_kind kind;
	size_t size;
	const ztd_char8_t* name;
} ztdc_thrd_attr_c8name_sized;

typedef struct ztdc_thrd_attr_c16name {
	ztdc_thrd_attr_kind kind;
	const ztd_char16_t* name;
} ztdc_thrd_attr_c16name;

typedef struct ztdc_thrd_attr_c16name_sized {
	ztdc_thrd_attr_kind kind;
	size_t size;
	const ztd_char16_t* name;
} ztdc_thrd_attr_c16name_sized;

typedef struct ztdc_thrd_attr_c32name {
	ztdc_thrd_attr_kind kind;
	const ztd_char32_t* name;
} ztdc_thrd_attr_c32name;

typedef struct ztdc_thrd_attr_c32name_sized {
	ztdc_thrd_attr_kind kind;
	size_t size;
	const ztd_char32_t* name;
} ztdc_thrd_attr_c32name_sized;

typedef struct ztdc_thrd_attr_stack_size {
	ztdc_thrd_attr_kind kind;
	size_t size;
} ztdc_thrd_attr_stack_size;

typedef struct ztdc_thrd_attr_stack_storage {
	ztdc_thrd_attr_kind kind;
	size_t size;
	void* buffer;
} ztdc_thrd_attr_stack_storage;

typedef struct ztdc_thread_attr_stack_guard_size {
	ztdc_thrd_attr_kind kind;
	size_t size;
} ztdc_thrd_attr_stack_guard_size;

typedef struct ztdc_thrd_attr_detached {
	ztdc_thrd_attr_kind kind;
	bool detached;
} ztdc_thrd_attr_detached;

#endif
