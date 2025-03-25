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

extern "C" inline int thrd_main(void* arg) {
	int t_id           = *(int*)arg;
	char name_buf[128] = {};
	int success        = ztdc_thrd_get_c8name(thrd_current(), sizeof(name_buf), (ztd_char8_t*)&name_buf[0]);
	if (success == thrd_success) {
		const char* t_name = name_buf;
		if (t_id == 1) {
			REQUIRE(std::strcmp(t_name, "meow?!") == 0);
		}
		else {
			REQUIRE(std::strcmp(t_name, "bark?!?!") == 0);
		}
	}
	thrd_exit(t_id);
}

TEST_CASE("thread test with new name/id checks", "[thrd][thrd_with_create_attrs]") {
	thrd_t t0 = {};
	thrd_t t1 = {};

	ztdc_thrd_attr_c16name name_attr = { // format
		ztdc_thrd_attr_kind_c16name, u"meow?!"
	};
	ztdc_thrd_attr_stack_size stack_size_attr = {
		ztdc_thrd_attr_kind_stack_size,
		1'024 * 16,
	};
	struct ztdc_thrd_attr_priority {
		ztdc_thrd_attr_kind kind;
		int priority;
	} priority_attr = {
		// some custom attribute or whatever
		(ztdc_thrd_attr_kind)INT_MAX,
		INT_MAX,
	};

	const ztdc_thrd_attr_kind* attrs[] = {
		&priority_attr.kind,
		&stack_size_attr.kind,
		&name_attr.kind,
	};

	int t0_id       = 1;
	int create_err0 = ztdc_thrd_create_attrs(&t0, thrd_main, &t0_id, ztdc_c_array_size(attrs), attrs);
	REQUIRE(create_err0 == thrd_success);

	int t1_id           = 2;
	int err_invocations = 0;
	name_attr.name      = u"bark?!?!";
	auto err_func       = [&](const ztdc_thrd_attr_kind kind, int err) -> int {
          REQUIRE(kind == priority_attr.kind);
          REQUIRE(err == thrd_error);
          ++err_invocations;
          return thrd_success;
	};
	auto err_func_trampoline = [](const ztdc_thrd_attr_kind kind, int err, void* userdata) -> int {
		return (*((decltype(err_func)*)userdata))(kind, err);
	};
	int create_err1 = ztdc_thrd_create_attrs_err(
	     &t1, thrd_main, &t1_id, ztdc_c_array_size(attrs), attrs, err_func_trampoline, &err_func);
	REQUIRE(create_err1 == thrd_success);

	int res0 = 0;
	int res1 = 0;
	thrd_join(t0, &res0);
	thrd_join(t1, &res1);
	REQUIRE(res0 == 1);
	REQUIRE(res1 == 2);
	REQUIRE(err_invocations == 1);
}
