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
#include <limits.h>

inline static int thread_main(void* arg) {
	int t_id           = *(int*)arg;
	char name_buf[128] = { 0 };
	ztdc_thrd_get_c8name(thrd_current(), sizeof(name_buf), (unsigned char*)name_buf);
	const char* t_name = name_buf;
	printf("thread id: %d\nthread name: %s\n\n", t_id, t_name);
	thrd_exit(t_id);
}

int thread_handle_attribute_errors(const ztdc_thrd_attr_kind* attr_kind, int err, void* userdata) {
	(void)userdata;
	// check for the unrecognized attribute that triggers an error
	ZTD_ASSERT(*attr_kind == (ztdc_thrd_attr_kind)0x12345678);
	ZTD_ASSERT(err == thrd_error);
	// returning thrd_success means "it's okay, ignore the attribute error"
	return thrd_success;
}

int main(void) {
	thrd_t t0 = { 0 };

	ztdc_thrd_attr__c32name name_attr = { // format
		.kind = ztdc_thrd_attr_kind__c32name,
		.name = U"meow?!"
	};
	ztdc_thrd_attr_stack_size stack_size_attr = {
		.kind = ztdc_thrd_attr_kind_stack_size,
		.size = 1024 * 16,
	};
	const struct ztdc_thrd_attr_priority {
		ztdc_thrd_attr_kind kind;
		int priority;
	} priority_attr = {
		// some custom attribute or whatever
		.kind     = (ztdc_thrd_attr_kind)0x12345678,
		.priority = INT_MAX,
	};

	const ztdc_thrd_attr_kind* attrs[] = {
		&priority_attr.kind,
		&stack_size_attr.kind,
		&name_attr.kind,
	};

	int t0_id      = 0;
	int create_err = ztdc_thrd_create_attrs_err(
	     &t0, thread_main, &t0_id, ztdc_c_array_size(attrs), attrs, thread_handle_attribute_errors, NULL);
	ZTD_ASSERT(create_err == thrd_success);

	int res0 = 0;
	thrd_join(t0, &res0);
	ZTD_ASSERT(res0 == 0);
	return 0;
}
