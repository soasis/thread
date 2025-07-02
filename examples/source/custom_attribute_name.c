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

#include <ztd/thread.h>
#include <ztd/idk/size.h>
#include <ztd/idk/assert.h>

#include <stdio.h>
#if ZTD_IS_ON(ZTD_PLATFORM_MAC_OS)
#include <pthread.h>
#elif ZTD_IS_ON(ZTD_C_STDLIB_BSD) && ZTD_IS_ON(ZTD_PLATFORM_NET_BSD)
#include <pthread.h>
#elif ZTD_IS_ON(ZTD_C_STDLIB_BSD) && (ZTD_IS_ON(ZTD_PLATFORM_FREE_BSD) || ZTD_IS_ON(ZTD_PLATFORM_OPEN_BSD))
#include <pthread.h>
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1
#include <Windows.h>
#endif

enum { EXPECTED_MARKER_VALUE = 32 };

inline static int thrd_custom_name(thrd_t t, ztdc_thrd_native_handle_t t_handle, ztdc_thrd_id_t t_id, void* userdata) {
	(void)t;
	(void)t_handle;
	(void)t_id;
	int custom_marker = *((const int*)userdata);
	printf("thrd_custom_name called with custom maker: %d\n", custom_marker);
	if (custom_marker != EXPECTED_MARKER_VALUE) {
		return thrd_error;
	}
#if ZTD_IS_ON(ZTD_PLATFORM_MAC_OS)
	pthread_setname_np("meow!");
#elif ZTD_IS_ON(ZTD_C_STDLIB_BSD) && ZTD_IS_ON(ZTD_PLATFORM_NET_BSD)
	pthread_setname_np(t_handle, "meow!", NULL);
#elif ZTD_IS_ON(ZTD_C_STDLIB_BSD) && (ZTD_IS_ON(ZTD_PLATFORM_FREE_BSD) || ZTD_IS_ON(ZTD_PLATFORM_OPEN_BSD))
	pthread_set_name_np(t_handle, "meow!");
#elif ZTD_IS_ON(ZTD_PLATFORM_WINDOWS)
	SetThreadDescription(t_handle, L"meow!");
#endif
	return thrd_success;
}

inline static int thrd_main(void* arg) {
	int t_id           = *(int*)arg;
	char name_buf[128] = { 0 };
	ztdc_thrd_get_c8name(thrd_current(), sizeof(name_buf), (unsigned char*)name_buf);
	const char* t_name = name_buf;
	printf("thread id: %d\nthread name: %s\n\n", t_id, t_name);
	thrd_exit(t_id);
}

int main(void) {
	thrd_t t0 = { 0 };

	int custom_marker = EXPECTED_MARKER_VALUE;

	const ztdc_thrd_attr_custom_on_new custom_attr = {
		.kind     = ztdc_thrd_attr_kind_custom_on_new,
		.func     = thrd_custom_name,
		.userdata = &custom_marker,
	};

	const ztdc_thrd_attr_kind* attrs[] = {
		&custom_attr.kind,
	};

	int t0_id       = 0;
	int create_err0 = ztdc_thrd_create_attrs(&t0, thrd_main, &t0_id, ztdc_c_array_size(attrs), attrs);
	ZTD_ASSERT(create_err0 == thrd_success);

	int res0 = 0;
	thrd_join(t0, &res0);
	ZTD_ASSERT(res0 == 0);
	return 0;
}
