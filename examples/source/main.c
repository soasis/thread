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

#include <stdio.h>

extern "C" inline int thrd_main(void* arg) {
	int t_id           = *(int*)arg;
	char name_buf[128] = {};
	ztdc_thrd_get_name(thrd_current(), sizeof(name_buf), (unsigned char*)name_buf);
	const char* t_name = name_buf;
	printf("thread id: %d\n", t_id);
	printf("thread name: %s\n", t_name);
	printf("\n");
	thrd_exit(t_id);
}

int main() {
	thrd_t t0 = {};
	thrd_t t1 = {};

	ztdc_thrd_attr_name name_attr = { // format
		.kind = ztdc_thrd_attr_kind_name,
		.name = "meow?!"
	};
	ztdc_thrd_attr_stack_size stack_size_attr = {
		.kind = ztdc_thrd_attr_kind_stack_size,
		.size = 1'024,
	};
	struct ztdc_thrd_attr_priority {
		ztdc_thrd_attr_kind kind;
		int priority;
	} priority_attr = {
		// some custom attribute or whatever
		.kind     = ztdc_thrd_attr_kind_impl_def_priority,
		.priority = INT_MAX,
	};

	ztdc_thrd_attr_kind* attrs[] = {
		&priority_attr.kind,
		&stack_size_attr.kind,
		&name_attr.kind,
	};

	int t0_id = 0;
	ztdc_thrd_create_attrs(&t0, thrd_main, &t0_id, ztdc_c_array_size(attrs), attrs);
	name_attr.name = "bark?!?!";
	int t1_id      = 1;
	ztdc_thrd_create_attrs(&t1, thrd_main, &t1_id, ztdc_c_array_size(attrs), attrs);

	int res0 = 0;
	int res1 = 0;
	thrd_join(t0, &res0);
	thrd_join(t1, &res1);
	return res0 + res1;
}
