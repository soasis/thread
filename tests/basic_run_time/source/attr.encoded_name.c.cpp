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

TEST_CASE("encoded thread name check", "[thrd][thrd_with_create_attrs][encoded-name]") {
#define MAKE_TEST_BRACKET(prefix, given, expected, expected_prefix)                                                                           \
	using given_char_t    = std::remove_cv_t<std::remove_reference_t<decltype(given[0])>>;                                                   \
	using expected_char_t = std::remove_cv_t<std::remove_reference_t<decltype(expected[0])>>;                                                \
	static constexpr const given_char_t thread_name[]             = given;                                                                   \
	static constexpr const expected_char_t expected_thread_name[] = expected;                                                                \
	const constexpr auto thrd_main                                = [](void* arg) -> int {                                                   \
          int t_id                      = *(int*)arg;                                                          \
          expected_char_t name_buf[128] = {};                                                                  \
          int name_get_result                                                                                  \
               = ztdc_thrd_get_##expected_prefix##name(thrd_current(), ztdc_c_array_size(name_buf), name_buf); \
          REQUIRE(name_get_result == thrd_success);                                                            \
          const expected_char_t* name = name_buf;                                                              \
          int expected_name_result                                                                             \
               = std::memcmp(name, &expected_thread_name[0], ztdc_c_array_size(expected_thread_name));         \
          REQUIRE(expected_name_result == 0);                                                                  \
          thrd_exit(t_id);                                                                                     \
	};                                                                                                                                       \
                                                                                                                                              \
	thrd_t t0 = {};                                                                                                                          \
                                                                                                                                              \
	ztdc_thrd_attr_##prefix##name name_attr = {                                                                                              \
		/* format */                                                                                                                        \
		ztdc_thrd_attr_kind_##prefix##name,                                                                                                 \
		thread_name,                                                                                                                        \
	};                                                                                                                                       \
	struct ztdc_thrd_attr_priority {                                                                                                         \
		ztdc_thrd_attr_kind kind;                                                                                                           \
		int priority;                                                                                                                       \
	} priority_attr = {                                                                                                                      \
		ztdc_thrd_attr_kind_impl_def_priority,                                                                                              \
		INT_MAX,                                                                                                                            \
	};                                                                                                                                       \
                                                                                                                                              \
	ztdc_thrd_attr_kind* attrs[] = {                                                                                                         \
		&priority_attr.kind,                                                                                                                \
		&name_attr.kind,                                                                                                                    \
	};                                                                                                                                       \
                                                                                                                                              \
	int t0_id = 0xF3;                                                                                                                        \
	ztdc_thrd_create_attrs(&t0, thrd_main, &t0_id, ztdc_c_array_size(attrs), attrs);                                                         \
	int res0 = 0;                                                                                                                            \
	thrd_join(t0, &res0);                                                                                                                    \
	REQUIRE(res0 == 0xF3)

	SECTION("c8/u8/ascii") {
		MAKE_TEST_BRACKET(c8, u8"meow?!", u8"meow?!", c8);
	}
	SECTION("c16/u/ascii") {
		MAKE_TEST_BRACKET(c16, u"meow?!", u8"meow?!", c8);
	}
	SECTION("c32/U/ascii") {
		MAKE_TEST_BRACKET(c32, U"meow?!", u8"meow?!", c8);
	}
	SECTION("mc//ascii") {
		MAKE_TEST_BRACKET(mc, "meow?!", u8"meow?!", c8);
	}
	SECTION("mwc/L/ascii") {
		MAKE_TEST_BRACKET(mwc, L"meow?!", u8"meow?!", c8);
	}
	SECTION("c8/u8/unicode") {
		MAKE_TEST_BRACKET(
		     c8, u8"\u300E\U0001F49A yay! \U0001F49A\u300F", u8"\u300E\U0001F49A yay! \U0001F49A\u300F", c8);
	}
	SECTION("c16/u/unicode") {
		MAKE_TEST_BRACKET(
		     c16, u"\u300E\U0001F49A yay! \U0001F49A\u300F", u8"\u300E\U0001F49A yay! \U0001F49A\u300F", c8);
	}
	SECTION("c32/U/unicode") {
		MAKE_TEST_BRACKET(
		     c32, U"\u300E\U0001F49A yay! \U0001F49A\u300F", u8"\u300E\U0001F49A yay! \U0001F49A\u300F", c8);
	}

	SECTION("c8/u8/ascii") {
		MAKE_TEST_BRACKET(c8, u8"meow?!", u"meow?!", c16);
	}
	SECTION("c16/u/ascii") {
		MAKE_TEST_BRACKET(c16, u"meow?!", u"meow?!", c16);
	}
	SECTION("c32/U/ascii") {
		MAKE_TEST_BRACKET(c32, U"meow?!", u"meow?!", c16);
	}
	SECTION("mc//ascii") {
		MAKE_TEST_BRACKET(mc, "meow?!", u"meow?!", c16);
	}
	SECTION("mwc/L/ascii") {
		MAKE_TEST_BRACKET(mwc, L"meow?!", u"meow?!", c16);
	}
	SECTION("c8/u8/unicode") {
		MAKE_TEST_BRACKET(
		     c8, u8"\u300E\U0001F49A yay! \U0001F49A\u300F", u"\u300E\U0001F49A yay! \U0001F49A\u300F", c16);
	}
	SECTION("c16/u/unicode") {
		MAKE_TEST_BRACKET(
		     c16, u"\u300E\U0001F49A yay! \U0001F49A\u300F", u"\u300E\U0001F49A yay! \U0001F49A\u300F", c16);
	}
	SECTION("c32/U/unicode") {
		MAKE_TEST_BRACKET(
		     c32, U"\u300E\U0001F49A yay! \U0001F49A\u300F", u"\u300E\U0001F49A yay! \U0001F49A\u300F", c16);
	}

	SECTION("c8/u8/ascii") {
		MAKE_TEST_BRACKET(c8, u8"meow?!", U"meow?!", c32);
	}
	SECTION("c16/u/ascii") {
		MAKE_TEST_BRACKET(c16, u"meow?!", U"meow?!", c32);
	}
	SECTION("c32/U/ascii") {
		MAKE_TEST_BRACKET(c32, U"meow?!", U"meow?!", c32);
	}
	SECTION("mc//ascii") {
		MAKE_TEST_BRACKET(mc, "meow?!", U"meow?!", c32);
	}
	SECTION("mwc/L/ascii") {
		MAKE_TEST_BRACKET(mwc, L"meow?!", U"meow?!", c32);
	}
	SECTION("c8/u8/unicode") {
		MAKE_TEST_BRACKET(
		     c8, u8"\u300E\U0001F49A yay! \U0001F49A\u300F", U"\u300E\U0001F49A yay! \U0001F49A\u300F", c32);
	}
	SECTION("c16/u/unicode") {
		MAKE_TEST_BRACKET(
		     c16, u"\u300E\U0001F49A yay! \U0001F49A\u300F", U"\u300E\U0001F49A yay! \U0001F49A\u300F", c32);
	}
	SECTION("c32/U/unicode") {
		MAKE_TEST_BRACKET(
		     c32, U"\u300E\U0001F49A yay! \U0001F49A\u300F", U"\u300E\U0001F49A yay! \U0001F49A\u300F", c32);
	}
}
