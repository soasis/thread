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

#ifndef ZTD_THREAD_THREADS_ATTR_H
#define ZTD_THREAD_THREADS_ATTR_H

#include <ztd/thread/version.h>

#include <ztd/thread/threads.native.h>

#include <ztd/idk/charN_t.h>

#if ZTD_IS_ON(ZTD_CXX)
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif



/// @brief A function used to modify a thread after creation but before the launch of its primary function.
///
/// @param[in] __thr The recently successfully created thread handle.
/// @param[in] __handle The native thread handle. It is completely implementation-defined what this value is.
/// @param[in] __id The native thread ID. May not have a meaningful value. It is completely implementation-defined what
/// this value is.
/// @param[in] __userdata The user data provided for this function.
///
/// @return One of the thread error codes on failure, or `thrd_success` if whatever action was taken was successfully
/// applied. If it returns anything but `thrd_success`, the `thrd_t` may be destroyed if the `err_func` that processes
/// this return value also returns a non-success error itself.
///
/// @remarks The return values allowed are one of the thread error codes. The value that closest reflects the issue
/// should be returned, otherwise success is signaled solely by returning `thrd_success`.
typedef int(ztdc_thrd_attr_func_t)(
     thrd_t __thr, ztdc_thrd_native_handle_t __handle, ztdc_thrd_id_t __id, void* __userdata);

/// @brief An enumeration containing all of the standard and implementation-defined thread attributes.
///
/// @remark The first [0, 65'535] for the standard, and 2,147,418,112 reserved values [65'636, 2'147'483'647] for
/// implementation-defined attributes. Each value of the enumeration corresponds to a structure that describes something
/// for the thread to use.
typedef enum ztdc_thrd_attr_kind
#if ZTD_IS_ON(ZTD_CXX) || (ZTD_IS_ON(ZTD_C) && __STDC_VERSION__ >= 202300L)
: int_least32_t
#endif
{
	/// @brief A raw, direct name copied unmodified into the thread name descriptor. Corresponds to the structure
	/// ztdc_thrd_attr_name_sized.
	ztdc_thrd_attr_kind_name = 0,
	/// @brief A raw, direct name copied unmodified into the thread name descriptor. Includes the size. Corresponds to
	/// the structure ztdc_thrd_attr_name_sized.
	ztdc_thrd_attr_kind_name_sized = 1,
	/// @brief A name provided in the execution encoding to be transformed and stored as the thread name. Corresponds
	/// to the structure ztdc_thrd_attr_mcname.
	ztdc_thrd_attr_kind_mcname = 2,
	/// @brief A name provided in the execution encoding to be transformed and stored as the thread name. Includes the
	/// size. Corresponds to the structure ztdc_thrd_attr_mcname_sized.
	ztdc_thrd_attr_kind_mcname_sized = 3,
	/// @brief A name provided in the execution encoding to be transformed and stored as the thread name. Corresponds
	/// to the structure ztdc_thrd_attr_mwcname.
	ztdc_thrd_attr_kind_mwcname = 4,
	/// @brief A name provided in the execution encoding to be transformed and stored as the thread name. Includes the
	/// size. Corresponds to the structure ztdc_thrd_attr_mwcname_sized.
	ztdc_thrd_attr_kind_mwcname_sized = 5,
	/// @brief A name provided in the UTF-8 encoding to be transformed and stored as the thread name. Corresponds
	/// to the structure ztdc_thrd_attr_c8name.
	ztdc_thrd_attr_kind_c8name = 6,
	/// @brief A name provided in the UTF-8 encoding to be transformed and stored as the thread name. Includes the
	/// size. Corresponds to the structure ztdc_thrd_attr_c8name_sized.
	ztdc_thrd_attr_kind_c8name_sized = 7,
	/// @brief A name provided in the UTF-16 encoding to be transformed and stored as the thread name. Corresponds
	/// to the structure ztdc_thrd_attr_c16name.
	ztdc_thrd_attr_kind_c16name = 8,
	/// @brief A name provided in the execution encoding to be transformed and stored as the thread name. Includes the
	/// size. Corresponds to the structure ztdc_thrd_attr_c16name_sized.
	ztdc_thrd_attr_kind_c16name_sized = 9,
	/// @brief A name provided in the UTF-32 encoding to be transformed and stored as the thread name. Corresponds to
	/// the structure ztdc_thrd_attr_c32name.
	ztdc_thrd_attr_kind_c32name = 10,
	/// @brief A name provided in the UTF-32 encoding to be transformed and stored as the thread name. Includes the
	/// size. Corresponds to the structure ztdc_thrd_attr_c32name_sized.
	ztdc_thrd_attr_kind_c32name_sized = 11,
	/// @brief The expected stack size for the implementation. Corresponds to the structure ztdc_thrd_attr_stack_size.
	ztdc_thrd_attr_kind_stack_size = 32,
	/// @brief Whether or not this thread should be started detached. Corresponds to the structure
	/// ztdc_thrd_attr_detached.
	ztdc_thrd_attr_kind_detached = 256,
	/// @brief A custom function that is called on the originating thread to perform any custom actions on the thread.
	/// Gives the newly created thread, the native thread handle, and the native thread ID alongside a user data. Is
	/// invoked after all other thread attributes are applied.
	ztdc_thrd_attr_kind_custom_on_origin = 512,
	/// @brief A custom function that is called on the new thread to perform any custom actions on the thread.
	/// Gives the newly created thread, the native thread handle, and the native thread ID alongside a user data. Is
	/// invoked after all other thread attributes are applied.
	ztdc_thrd_attr_kind_custom_on_new = 513,
	/// @brief The last standard-defined attribute that marks the beginning of the implementation-defined attributes.
	/// Corresponds to nothing and is just for informational purposes.
	ztdc_thrd_attr_kind_implementation_defined = 0xFFFF,
	/// @brief A pointer and a size representing the whole piece of memory to use for the stack. Implementation-defined
	/// and not cross-platform. Corresponds to the structure ztdc_thrd_attr__stack_storage.
	ztdc_thrd_attr_kind__stack_storage = 0x10020,
	/// @brief A size dictating how big the guard area on one or both sides of the stack might be.
	/// Implementation-defined and not cross-platform. Corresponds to the structure ztdc_thread_attr__stack_guard_size.
	ztdc_thrd_attr_kind__stack_guard_size = 0x10021,
} ztdc_thrd_attr_kind;

/// @brief Describes a direct, raw name for the implementation to use without modification.
typedef struct ztdc_thrd_attr_name {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_name.
	ztdc_thrd_attr_kind kind;
	/// @brief A pointer to the data that represents the name. It is null-terminated with an implementation-defined
	/// number of bytes representing that null termination. How many bytes make up a valid null terminator is
	/// determined by the implementation.
	///
	/// @remarks For Windows, this is typically 2 bytes (16 bits). For POSIX-derivative implementations, this is
	/// usually one byte (8 bits). On some exotic implementations, it can be four bytes (32 bits).
	const void* name;
} ztdc_thrd_attr_name;

/// @brief Describes a direct, raw name for the implementation to use without modification. Includes the size.
typedef struct ztdc_thrd_attr_name_sized {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_name_sized.
	ztdc_thrd_attr_kind kind;
	/// @brief The size of the data pointer. For the range denoted by the pointer `name` and this `size` parameter, it
	/// must not contain one of the implementation-defined null terminators for the data within.
	size_t size;
	/// @brief A pointer to the data that represents the name. How many bytes make up a valid null terminator is
	/// determined by the implementation.
	///
	/// @remarks For Windows, this is typically 2 bytes (16 bits). For POSIX-derivative implementations, this is
	/// usually one byte (8 bits). On some exotic implementations, it can be four bytes (32 bits).
	const void* name;
} ztdc_thrd_attr_name_sized;

/// @brief Describes a name encoded in the execution encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable.
typedef struct ztdc_thrd_attr_mcname {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_mcname.
	ztdc_thrd_attr_kind kind;
	/// @brief The execution encoding name.
	const char* name;
} ztdc_thrd_attr_mcname;

/// @brief Describes a name encoded in the execution encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable. Includes the size.
typedef struct ztdc_thrd_attr_mcname_sized {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_mcname_sized.
	ztdc_thrd_attr_kind kind;
	/// @brief The size of the name. Must not include a null terminator in its range.
	size_t size;
	/// @brief The execution encoding name.
	const char* name;
} ztdc_thrd_attr_mcname_sized;

/// @brief Describes a name encoded in the wide execution encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable.
typedef struct ztdc_thrd_attr_mwcname {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_mwcname.
	ztdc_thrd_attr_kind kind;
	/// @brief The wide execution encoding name.
	const ztd_wchar_t* name;
} ztdc_thrd_attr_mwcname;

/// @brief Describes a name encoded in the wide execution encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable. Includes the size.
typedef struct ztdc_thrd_attr_mwcname_sized {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_mwcname_sized.
	ztdc_thrd_attr_kind kind;
	/// @brief The size of the name. Must not include a null terminator in its range.
	size_t size;
	/// @brief The wide execution encoding name.
	const ztd_wchar_t* name;
} ztdc_thrd_attr_mwcname_sized;

/// @brief Describes a name encoded in the UTF-8 encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable.
typedef struct ztdc_thrd_attr_c8name {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_c8name.
	ztdc_thrd_attr_kind kind;
	/// @brief The UTF-8 encoding name. Null terminated.
	const ztd_char8_t* name;
} ztdc_thrd_attr_c8name;

/// @brief Describes a name encoded in the UTF-8 encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable. Includes the size.
typedef struct ztdc_thrd_attr_c8name_sized {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_c8name_sized.
	ztdc_thrd_attr_kind kind;
	/// @brief The size of the name. Must not include a null terminator in its range.
	size_t size;
	/// @brief The UTF-8 encoding name.
	const ztd_char8_t* name;
} ztdc_thrd_attr_c8name_sized;

/// @brief Describes a name encoded in the UTF-16 encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable.
typedef struct ztdc_thrd_attr_c16name {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_c16name.
	ztdc_thrd_attr_kind kind;
	/// @brief The UTF-8 encoding name.
	const ztd_char16_t* name;
} ztdc_thrd_attr_c16name;

/// @brief Describes a name encoded in the UTF-16 encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable. Includes the size.
typedef struct ztdc_thrd_attr_c16name_sized {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_c16name_sized.
	ztdc_thrd_attr_kind kind;
	/// @brief The size of the name. Must not include a null terminator in its range.
	size_t size;
	/// @brief The UTF-8 encoding name.
	const ztd_char16_t* name;
} ztdc_thrd_attr_c16name_sized;

/// @brief Describes a name encoded in the UTF-32 encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable.
typedef struct ztdc_thrd_attr_c32name {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_c32name.
	ztdc_thrd_attr_kind kind;
	/// @brief The UTF-32 encoding name. Null terminated.
	const ztd_char32_t* name;
} ztdc_thrd_attr_c32name;

/// @brief Describes a name encoded in the UTF-32 encoding for the implementation to transcode to the a suitable
/// internally-named string, if applicable. Includes the size.
typedef struct ztdc_thrd_attr_c32name_sized {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_c32name_sized.
	ztdc_thrd_attr_kind kind;
	/// @brief The size of the name. Must not include a null terminator in its range.
	size_t size;
	/// @brief The UTF-32 encoding name.
	const ztd_char32_t* name;
} ztdc_thrd_attr_c32name_sized;

/// @brief Describes a request for a specific stack size.
typedef struct ztdc_thrd_attr_stack_size {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_stack_size.
	ztdc_thrd_attr_kind kind;
	/// @brief The size of the stack space. Some implementations produce an error when requested and the size is too
	/// large or small. Other implementations will round the size up if it is too small. Use ztdc_thrd_create_attrs_err
	/// to detect whether or not this parameter has taken successfully.
	size_t size;
} ztdc_thrd_attr_stack_size;

/// @brief Describes whether or not the thread should start detached or not.
typedef struct ztdc_thrd_attr_detached {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_detached.
	ztdc_thrd_attr_kind kind;
	/// @brief Whether or not the thread should be spawned immediately detached or not.
	bool detached;
} ztdc_thrd_attr_detached;

/// @brief Describes an attribute that will invoke a custom function on the newly created thread object.
///
/// @remarks This is invoked on the originating thread. `func` will never be invoked concurrently with
/// ztdc_thrd_attr_custom_on_new. This always runs after ztdc_thrd_attr_custom_on_origin if both of these are present.
typedef struct ztdc_thrd_attr_custom_on_origin {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_custom_on_origin.
	ztdc_thrd_attr_kind kind;
	/// @brief The function to process the native thread handle and native thread id.
	///
	/// @remarks This function is always invoked on the originating thread that is creating the new thread. To invoke
	/// this function on the newly created thread instead rather than the originating thread, just before the thread
	/// function is called, see ztdc_thrd_attr_custom_on_new.
	ztdc_thrd_attr_func_t* func;
	/// @brief The userdata to pass to `func`.
	void* userdata;
} ztdc_thrd_attr_custom_on_origin;

/// @brief Describes an attribute that will invoke a custom function on the newly created thread object.
///
/// @remarks This is invoked on the new thread. `func` will never be invoked concurrently with
/// ztdc_thrd_attr_custom_on_origin. This always runs before ztdc_thrd_attr_custom_on_origin.
typedef struct ztdc_thrd_attr_custom_on_new {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind_custom_on_new.
	ztdc_thrd_attr_kind kind;
	/// @brief The function to process the thread, native thread handle and native thread id.
	///
	/// @remarks This function is always invoked on the thread of the function that is creating the thread. To invoke
	/// this function on the originating thread, just before the thread function is called, see
	/// ztdc_thrd_attr_custom_on_origin.
	ztdc_thrd_attr_func_t* func;
	/// @brief The user data argument to pass to `func`.
	void* userdata;
} ztdc_thrd_attr_custom_on_new;

/// @brief Implementation-defined stack buffer attribute. Works on typical POSIX thread implementations.
typedef struct ztdc_thrd_attr__stack_storage {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind__stack_storage.
	ztdc_thrd_attr_kind kind;
	/// @brief The size of the buffer to be used as the stack for the thread.
	size_t size;
	/// @brief The size of the
	void* buffer;
} ztdc_thrd_attr__stack_storage;

/// @brief Implementation-defined stack guard size attribute. Works only on typical POSIX thread implementations.
typedef struct ztdc_thread_attr__stack_guard_size {
	/// @brief The kind of the attribute. Must be ztdc_thrd_attr_kind__stack_guard_size.
	ztdc_thrd_attr_kind kind;
	/// @brief The size of the stack's guard, in bytes. Can be placed at just the start of the stack's reserved space,
	/// or the beginning and the end of the stack's reserved/committed space (it is determined by the implementation).
	size_t size;
} ztdc_thrd_attr__stack_guard_size;

#endif
