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

#ifndef ZTD_THREAD_SOURCE_DETAIL_THREADS_PTHREADS_IMPLEMENTATION_H
#define ZTD_THREAD_SOURCE_DETAIL_THREADS_PTHREADS_IMPLEMENTATION_H

#include <ztd/thread/version.h>

#define _GNU_SOURCE
#include <pthread.h>
#if ZTD_IS_ON(ZTD_HEADER_PTHREAD_NP_H)
#include <pthread_np.h>
#endif

ZTD_USE(ZTD_C_LANGUAGE_LINKAGE)
ZTD_USE(ZTD_THREAD_API_INTERNAL_LINKAGE)
int __ztdc_pthread_to_thread_error(int __code);

#endif
