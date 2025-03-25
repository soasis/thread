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

#include <catch2/catch_all.hpp>

#include <ztd/idk/size.hpp>

#include <cstring>
#include <cstdio>
#include <type_traits>

TEST_CASE("thread raw name check", "[thrd][thrd_with_create_attrs][name]") {
	static constexpr unsigned char given[]                = "abcdefghi\0\0\0";
	static constexpr unsigned char expected_thread_name[] = "abcdefghi\0\0\0";
	const constexpr auto thrd_main                        = [](void* arg) -> int {
          int t_id                    = *(int*)arg;
          unsigned char name_buf[128] = {};
          int name_get_result         = ztdc_thrd_get_name(thrd_current(), ztdc_c_array_size(name_buf), name_buf);
          REQUIRE(name_get_result == thrd_success);
          const unsigned char* name = name_buf;
          int expected_name_result  = std::memcmp(name, &expected_thread_name[0], sizeof(expected_thread_name));
          REQUIRE(expected_name_result == 0);
          thrd_exit(t_id);
	};

	thrd_t t0 = {};

	ztdc_thrd_attr_name name_attr = {
		/* format */
		ztdc_thrd_attr_kind_name,
		given,
	};

	const ztdc_thrd_attr_kind* attrs[] = {
		&name_attr.kind,
	};

	int t0_id      = 0xF3;
	int create_err = ztdc_thrd_create_attrs(&t0, thrd_main, &t0_id, ztdc_c_array_size(attrs), attrs);
	REQUIRE(create_err == thrd_success);
	int res0 = 0;
	thrd_join(t0, &res0);
	REQUIRE(res0 == 0xF3);
}
