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

namespace {
	typedef struct thread_results {
		int id;
		int getname_result;
		int strcmp_result;
	} thread_results;

	inline int thread_main(void* arg) {
		thread_results* t_results = (thread_results*)arg;
		char name_buf[128]        = {};
		t_results += 1;
		t_results->getname_result
		     = ztdc_thrd_get_c8name(thrd_current(), sizeof(name_buf), (ztd_char8_t*)&name_buf[0]);
		if (t_results->getname_result == thrd_success) {
			const char* t_name = name_buf;
			if (t_results->id == 1) {
				t_results->strcmp_result = std::strcmp(t_name, "meow?!");
			}
			else {
				t_results->strcmp_result = std::strcmp(t_name, "bark?!?!");
			}
		}
		thrd_exit(t_results->id);
	}

	int thread_error_handle(const ztdc_thrd_attr_kind* kind, int err, void* userdata) {
		(void)kind;
		(void)err;
		int* p_err_invocations = (int*)userdata;
		*p_err_invocations += 1;
		return thrd_error;
	}
} // namespace

TEST_CASE("thread test with attribute failure", "[thrd][thrd_with_create_attrs_err][fail]") {
	thrd_t t0 = {};
	thrd_t t1 = {};

	ztdc_thrd_attr__c16name name_attr = { // format
		ztdc_thrd_attr_kind__c16name, u"meow?!"
	};
	ztdc_thrd_attr_stack_size stack_size_attr = {
		ztdc_thrd_attr_kind_stack_size,
		ZTDC_THRD_MINIMUM_STACK_SIZE,
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

	thread_results t0_results = { 0, 0xFFFF, 0xFFFF };
	int err_invocations0      = 0;
	int create_err0           = ztdc_thrd_create_attrs_err(
          &t0, thread_main, &t0_results, ztdc_c_array_size(attrs), attrs, thread_error_handle, &err_invocations0);
	REQUIRE(create_err0 == thrd_error);
	REQUIRE(err_invocations0 == 1);

	thread_results t1_results = { 1, 0xFFFF, 0xFFFF };
	int err_invocations1      = 0;
	name_attr.name            = u"bark?!?!";
	auto err_func             = [&](const ztdc_thrd_attr_kind* kind, int err) -> int {
          REQUIRE(*kind == priority_attr.kind);
          REQUIRE(err == thrd_error);
          err_invocations1 += 1;
          return thrd_error;
	};
	auto err_func_trampoline = [](const ztdc_thrd_attr_kind* kind, int err, void* userdata) -> int {
		return (*((decltype(err_func)*)userdata))(kind, err);
	};
	int create_err1 = ztdc_thrd_create_attrs_err(
	     &t1, thread_main, &t1_results, ztdc_c_array_size(attrs), attrs, err_func_trampoline, &err_func);
	REQUIRE(create_err1 == thrd_error);
	REQUIRE(err_invocations1 == 1);

	REQUIRE(t0_results.id == 0);
	REQUIRE(t0_results.getname_result == 0xFFFF);
	REQUIRE(t0_results.strcmp_result == 0xFFFF);
	REQUIRE(t1_results.id == 1);
	REQUIRE(t1_results.getname_result == 0xFFFF);
	REQUIRE(t1_results.strcmp_result == 0xFFFF);
}
