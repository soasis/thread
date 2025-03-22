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

#define ZTD_THREAD_VERSION_MAJOR 0
#define ZTD_THREAD_VERSION_MINOR 0
#define ZTD_THREAD_VERSION_PATCH 0
#define ZTD_THREAD_VERSION_STRING "0.0.0"
#define ZTD_THREAD_VERSION \
	((ZTD_THREAD_VERSION_MAJOR * 100000) + (ZTD_THREAD_VERSION_MINOR * 100) + (ZTD_THREAD_VERSION_PATCH))

#include <ztd/thread/detail/api.h>

#endif
