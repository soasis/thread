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

inline static int thread_custom_on_origin_attr(
     thrd_t t, ztdc_thrd_native_handle_t t_handle, ztdc_thrd_id_t t_id, void* userdata) {
	(void)t;
	(void)t_handle;
	(void)t_id;

	int* p_accumulator = (int*)userdata;
	*p_accumulator += 4;
	return thrd_success;
}

inline static int thread_custom_on_new_attr(
     thrd_t t, ztdc_thrd_native_handle_t t_handle, ztdc_thrd_id_t t_id, void* userdata) {
	(void)t;
	(void)t_handle;
	(void)t_id;

	int* p_accumulator = (int*)userdata;
	*p_accumulator *= 4;
	return thrd_success;
}

inline static int thread_main(void* arg) {
	int* p_accumulator  = (int*)arg;
	int passed_in_value = *p_accumulator;
	*p_accumulator *= 3;
	int modified_value = *p_accumulator;
	printf("passed-in accumulated value: %d\nmodified accumulated value: %d\n", passed_in_value, modified_value);
	return *p_accumulator * 5;
}

inline static int thread_attr_passthrough(const ztdc_thrd_attr_kind* kind, int err, void* userdata) {
	(void)kind;
	(void)userdata;
	return err;
}

int main(void) {
	thrd_t t0 = { 0 };

	enum { EXPECTED_ACCUMULATION_VALUE = 36, RETURNED_ACCUMULATION_VALUE = 180 };

	int accumulator = 3;

	const ztdc_thrd_attr_custom_on_new custom_on_new_attr = {
		.kind     = ztdc_thrd_attr_kind_custom_on_new,
		.func     = thread_custom_on_new_attr,
		.userdata = &accumulator,
	};

	const ztdc_thrd_attr_custom_on_origin custom_on_origin_attr = {
		.kind     = ztdc_thrd_attr_kind_custom_on_new,
		.func     = thread_custom_on_origin_attr,
		.userdata = &accumulator,
	};

	const ztdc_thrd_attr_kind* attrs[] = {
		&custom_on_origin_attr.kind,
		&custom_on_new_attr.kind,
	};

	int create_err0 = ztdc_thrd_create_attrs_err(
	     &t0, thread_main, &accumulator, ztdc_c_array_size(attrs), attrs, thread_attr_passthrough, NULL);
	ZTD_ASSERT(create_err0 == thrd_success);

	int res0 = 0;
	thrd_join(t0, &res0);
	ZTD_ASSERT(res0 == RETURNED_ACCUMULATION_VALUE);
	ZTD_ASSERT(accumulator == EXPECTED_ACCUMULATION_VALUE);

	return accumulator == EXPECTED_ACCUMULATION_VALUE ? 0 : 1;
}
