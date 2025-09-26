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

typedef struct thread_with_id {
	thrd_t thr;
	int id;
	int name_get_result0;
	int name_set_result0;
	int expected_name_result0;
	int name_get_result1;
	int name_set_result1;
	int expected_name_result1;
	int name_get_result2;
	int name_set_result2;
	int expected_name_result2;
} thread_with_id;

TEST_CASE("encoded thread name check", "[thrd][thrd_with_create_attrs][encoded-name]") {
#define MAKE_TEST_BRACKET(given_prefix, given, given_type, expected_prefix, expected, expected_type, impl_def)                 \
	using given_char_t    = std::remove_cv_t<std::remove_reference_t<decltype(given[0])>>;                                    \
	using expected_char_t = std::remove_cv_t<std::remove_reference_t<decltype(expected[0])>>;                                 \
	static constexpr const given_char_t thread_name[]             = given;                                                    \
	static constexpr const expected_char_t expected_thread_name[] = expected;                                                 \
	const constexpr auto thread_main0                             = [](void* arg) -> int {                                    \
          thread_with_id* t_id = (thread_with_id*)arg;                                             \
          {                                                                                        \
               expected_char_t name_buf[128] = {};                                                 \
               t_id->name_get_result0        = ztdc_thrd_get_##expected_prefix##name(              \
                    thrd_current(), ztdc_c_array_size(name_buf), (expected_type*)name_buf); \
               const expected_char_t* name = name_buf;                                             \
               t_id->expected_name_result0                                                         \
                    = std::memcmp(name, &expected_thread_name[0], sizeof(expected_thread_name));   \
          }                                                                                        \
          thrd_exit(t_id->id);                                                                     \
	};                                                                                                                        \
	const constexpr auto thread_main1 = [](void* arg) -> int {                                                                \
		thread_with_id* t_id = (thread_with_id*)arg;                                                                         \
		{                                                                                                                    \
			t_id->name_set_result0        = ztdc_thrd_set_##given_prefix##name(t_id->thr, (given_type*)thread_name);        \
			expected_char_t name_buf[128] = {};                                                                             \
			t_id->name_get_result0        = ztdc_thrd_get_##expected_prefix##name(                                          \
                    t_id->thr, ztdc_c_array_size(name_buf), (expected_type*)name_buf);                                  \
			const expected_char_t* name = name_buf;                                                                         \
			t_id->expected_name_result0                                                                                     \
			     = std::memcmp(name, &expected_thread_name[0], sizeof(expected_thread_name));                               \
		}                                                                                                                    \
		{                                                                                                                    \
			t_id->name_set_result1 = ztdc_thrd_set_##given_prefix##name_sized(                                              \
			     t_id->thr, ztdc_c_string_ptr_size(thread_name), (given_type*)thread_name);                                 \
			expected_char_t name_buf[128] = {};                                                                             \
			t_id->name_get_result1        = ztdc_thrd_get_##expected_prefix##name(                                          \
                    t_id->thr, ztdc_c_array_size(name_buf), (expected_type*)name_buf);                                  \
			const expected_char_t* name = name_buf;                                                                         \
			t_id->expected_name_result1                                                                                     \
			     = std::memcmp(name, &expected_thread_name[0], sizeof(expected_thread_name));                               \
		}                                                                                                                    \
		{                                                                                                                    \
			/* flex case where the actual string portion given is not null-terminated... */                                 \
			t_id->name_set_result2        = ztdc_thrd_set_##given_prefix##name_sized(t_id->thr,                             \
			            std::basic_string<given_char_t>(thread_name).find((given_char_t)'!'), (given_type*)thread_name);    \
			expected_char_t name_buf[128] = {};                                                                             \
			t_id->name_get_result2        = ztdc_thrd_get_##expected_prefix##name(                                          \
                    t_id->thr, ztdc_c_array_size(name_buf), (expected_type*)name_buf);                                  \
			const expected_char_t* name = name_buf;                                                                         \
			auto expected_name_size                                                                                         \
			     = std::basic_string<expected_char_t>(expected_thread_name).find((given_char_t)'!')                         \
			     * sizeof(expected_char_t);                                                                                 \
			t_id->expected_name_result2 = std::memcmp(name, &expected_thread_name[0], expected_name_size);                  \
		}                                                                                                                    \
		return t_id->id;                                                                                                     \
	};                                                                                                                        \
                                                                                                                               \
	thread_with_id t0_arg = { {}, 0xF3, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };             \
	ztdc_thrd_attr_##impl_def##given_prefix##name name_attr = {                                                               \
		/* format */                                                                                                         \
		ztdc_thrd_attr_kind_##impl_def##given_prefix##name,                                                                  \
		(given_type*)thread_name,                                                                                            \
	};                                                                                                                        \
	const ztdc_thrd_attr_kind* attrs[] = {                                                                                    \
		&name_attr.kind,                                                                                                     \
	};                                                                                                                        \
	int create_err = ztdc_thrd_create_attrs(&t0_arg.thr, thread_main0, &t0_arg, ztdc_c_array_size(attrs), attrs);             \
	REQUIRE(create_err == thrd_success);                                                                                      \
	int res0 = 0;                                                                                                             \
	thrd_join(t0_arg.thr, &res0);                                                                                             \
	REQUIRE(res0 == 0xF3);                                                                                                    \
	REQUIRE(res0 == t0_arg.id);                                                                                               \
	REQUIRE(t0_arg.name_get_result0 == thrd_success);                                                                         \
	REQUIRE(t0_arg.expected_name_result0 == 0);                                                                               \
                                                                                                                               \
	thread_with_id t1_arg = { {}, 0xF5, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };             \
	int create_err1       = ztdc_thrd_create_attrs(&t1_arg.thr, thread_main1, &t1_arg, 0, NULL);                              \
	REQUIRE(create_err1 == thrd_success);                                                                                     \
	int res1 = 0;                                                                                                             \
	thrd_join(t1_arg.thr, &res1);                                                                                             \
	REQUIRE(res1 == 0xF5);                                                                                                    \
	REQUIRE(res1 == t1_arg.id);                                                                                               \
	REQUIRE(t1_arg.name_set_result0 == thrd_success);                                                                         \
	REQUIRE(t1_arg.name_get_result0 == thrd_success);                                                                         \
	REQUIRE(t1_arg.expected_name_result0 == 0);                                                                               \
	REQUIRE(t1_arg.name_set_result1 == thrd_success);                                                                         \
	REQUIRE(t1_arg.name_get_result1 == thrd_success);                                                                         \
	REQUIRE(t1_arg.expected_name_result1 == 0);                                                                               \
	REQUIRE(t1_arg.name_set_result2 == thrd_success);                                                                         \
	REQUIRE(t1_arg.name_get_result2 == thrd_success);                                                                         \
	REQUIRE(t1_arg.expected_name_result2 == 0);

	SECTION("c8/u8/ascii") {
		MAKE_TEST_BRACKET(c8, u8"meow?!", ztd_char8_t, c8, u8"meow?!", ztd_char8_t, );
	}
	SECTION("c16/u/ascii") {
		MAKE_TEST_BRACKET(c16, u"meow?!", ztd_char16_t, c8, u8"meow?!", ztd_char8_t, _);
	}
	SECTION("c32/U/ascii") {
		MAKE_TEST_BRACKET(c32, U"meow?!", ztd_char32_t, c8, u8"meow?!", ztd_char8_t, _);
	}
	SECTION("mc//ascii") {
		MAKE_TEST_BRACKET(mc, "meow?!", ztd_char_t, c8, u8"meow?!", ztd_char8_t, );
	}
	SECTION("mwc/L/ascii") {
		MAKE_TEST_BRACKET(mwc, L"meow?!", ztd_wchar_t, c8, u8"meow?!", ztd_char8_t, );
	}
	SECTION("c8/u8/unicode") {
		MAKE_TEST_BRACKET(
		     c8, u8"\u300E\U0001F49Ayay!\u300F", ztd_char8_t, c8, u8"\u300E\U0001F49Ayay!\u300F", ztd_char8_t, );
	}
	SECTION("c16/u/unicode") {
		MAKE_TEST_BRACKET(
		     c16, u"\u300E\U0001F49Ayay!\u300F", ztd_char16_t, c8, u8"\u300E\U0001F49Ayay!\u300F", ztd_char8_t, _);
	}
	SECTION("c32/U/unicode") {
		MAKE_TEST_BRACKET(
		     c32, U"\u300E\U0001F49Ayay!\u300F", ztd_char32_t, c8, u8"\u300E\U0001F49Ayay!\u300F", ztd_char8_t, _);
	}

	SECTION("c8/u8/ascii") {
		MAKE_TEST_BRACKET(c8, u8"meow?!", ztd_char8_t, c16, u"meow?!", ztd_char16_t, );
	}
	SECTION("c16/u/ascii") {
		MAKE_TEST_BRACKET(c16, u"meow?!", ztd_char16_t, c16, u"meow?!", ztd_char16_t, _);
	}
	SECTION("c32/U/ascii") {
		MAKE_TEST_BRACKET(c32, U"meow?!", ztd_char32_t, c16, u"meow?!", ztd_char16_t, _);
	}
	SECTION("mc//ascii") {
		MAKE_TEST_BRACKET(mc, "meow?!", ztd_char_t, c16, u"meow?!", ztd_char16_t, );
	}
	SECTION("mwc/L/ascii") {
		MAKE_TEST_BRACKET(mwc, L"meow?!", ztd_wchar_t, c16, u"meow?!", ztd_char16_t, );
	}
	SECTION("c8/u8/unicode") {
		MAKE_TEST_BRACKET(
		     c8, u8"\u300E\U0001F49Ayay!\u300F", ztd_char8_t, c16, u"\u300E\U0001F49Ayay!\u300F", ztd_char16_t, );
	}
	SECTION("c16/u/unicode") {
		MAKE_TEST_BRACKET(
		     c16, u"\u300E\U0001F49Ayay!\u300F", ztd_char16_t, c16, u"\u300E\U0001F49Ayay!\u300F", ztd_char16_t, _);
	}
	SECTION("c32/U/unicode") {
		MAKE_TEST_BRACKET(
		     c32, U"\u300E\U0001F49Ayay!\u300F", ztd_char32_t, c16, u"\u300E\U0001F49Ayay!\u300F", ztd_char16_t, _);
	}

	SECTION("c8/u8/ascii") {
		MAKE_TEST_BRACKET(c8, u8"meow?!", ztd_char8_t, c32, U"meow?!", ztd_char32_t, );
	}
	SECTION("c16/u/ascii") {
		MAKE_TEST_BRACKET(c16, u"meow?!", ztd_char16_t, c32, U"meow?!", ztd_char32_t, _);
	}
	SECTION("c32/U/ascii") {
		MAKE_TEST_BRACKET(c32, U"meow?!", ztd_char32_t, c32, U"meow?!", ztd_char32_t, _);
	}
	SECTION("mc//ascii") {
		MAKE_TEST_BRACKET(mc, "meow?!", ztd_char_t, c32, U"meow?!", ztd_char32_t, );
	}
	SECTION("mwc/L/ascii") {
		MAKE_TEST_BRACKET(mwc, L"meow?!", ztd_wchar_t, c32, U"meow?!", ztd_char32_t, );
	}
	SECTION("c8/u8/unicode") {
		MAKE_TEST_BRACKET(
		     c8, u8"\u300E\U0001F49Ayay!\u300F", ztd_char8_t, c32, U"\u300E\U0001F49Ayay!\u300F", ztd_char32_t, );
	}
	SECTION("c16/u/unicode") {
		MAKE_TEST_BRACKET(
		     c16, u"\u300E\U0001F49Ayay!\u300F", ztd_char16_t, c32, U"\u300E\U0001F49Ayay!\u300F", ztd_char32_t, _);
	}
	SECTION("c32/U/unicode") {
		MAKE_TEST_BRACKET(
		     c32, U"\u300E\U0001F49Ayay!\u300F", ztd_char32_t, c32, U"\u300E\U0001F49Ayay!\u300F", ztd_char32_t, _);
	}

	SECTION("c8/u8/ascii") {
		MAKE_TEST_BRACKET(c8, u8"meow?!", ztd_char8_t, mc, "meow?!", ztd_char_t, );
	}
	SECTION("c16/u/ascii") {
		MAKE_TEST_BRACKET(c16, u"meow?!", ztd_char16_t, mc, "meow?!", ztd_char_t, _);
	}
	SECTION("c32/U/ascii") {
		MAKE_TEST_BRACKET(c32, U"meow?!", ztd_char32_t, mc, "meow?!", ztd_char_t, _);
	}
	SECTION("mc//ascii") {
		MAKE_TEST_BRACKET(mc, "meow?!", ztd_char_t, mc, "meow?!", ztd_char_t, );
	}
	SECTION("mwc/L/ascii") {
		MAKE_TEST_BRACKET(mwc, L"meow?!", ztd_wchar_t, mc, "meow?!", ztd_char_t, );
	}

	SECTION("c8/u8/ascii") {
		MAKE_TEST_BRACKET(c8, u8"meow?!", ztd_char8_t, mwc, L"meow?!", ztd_wchar_t, );
	}
	SECTION("c16/u/ascii") {
		MAKE_TEST_BRACKET(c16, u"meow?!", ztd_char16_t, mwc, L"meow?!", ztd_wchar_t, _);
	}
	SECTION("c32/U/ascii") {
		MAKE_TEST_BRACKET(c32, U"meow?!", ztd_char32_t, mwc, L"meow?!", ztd_wchar_t, _);
	}
	SECTION("mc//ascii") {
		MAKE_TEST_BRACKET(mc, "meow?!", ztd_char_t, mwc, L"meow?!", ztd_wchar_t, );
	}
	SECTION("mwc/L/ascii") {
		MAKE_TEST_BRACKET(mwc, L"meow?!", ztd_wchar_t, mwc, L"meow?!", ztd_wchar_t, );
	}
}
