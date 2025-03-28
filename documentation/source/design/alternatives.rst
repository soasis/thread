.. =============================================================================
..
.. ztd.thread
.. Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
.. Contact: opensource@soasis.org
..
.. Commercial License Usage
.. Licensees holding valid commercial ztd.thread licenses may use this file in
.. accordance with the commercial license agreement provided with the
.. Software or, alternatively, in accordance with the terms contained in
.. a written agreement between you and Shepherd's Oasis, LLC.
.. For licensing terms and conditions see your agreement. For
.. further information contact opensource@soasis.org.
..
.. Apache License Version 2 Usage
.. Alternatively, this file may be used under the terms of Apache License
.. Version 2.0 (the "License") for non-commercial use; you may not use this
.. file except in compliance with the License. You may obtain a copy of the
.. License at
..
.. https://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.
..
.. =============================================================================>

Alternative Designs?
====================

There are a number of alternative designs we decided not to go with for this library. Most of it was based on the experience of developing this library and what it may entail. In general, there were 3 other options that this library could have picked, trading some degree of type safety, ease of use, and similar. We believe only one of these other designs might have been somewhat workable, but



``_Generic`` For Type Safety?
-----------------------------

One of the issues with the current design is how type-unsafe it can be. When creating an object, without C++ constructors, it is impossible to set the ``kind`` enumeration value to the right object. For example:

.. code-block:: c

	#include <ztd/thread.h>

	int main () {
		ztdc_thrd_attr_c8name name = {
			.kind = ztdc_thrd_attr_kind_c32name, // MISTAKE
			.name = u8"meow"
		};

		// ...

		return 0;
	}

This is not fully type safe. C++'s design in `P2019 <https://wg21.link/p2019>`_ uses a templated constructor with each thread attribute being a type -- and without a ``ztdc_thrd_attr_kind``-style enumeration -- to solve this problem. It's infinitely more type safe. A ``_Generic`` approach could do the same for C, but the problem is that we do not have nearly enough in-language infrastructure to make this easy; any implementer having to iterate in a type-safe manner over a sequence of compile-time objects would end up pulling their hair out for hte final implementation, especially since that implementation necessarily has many deep complex ties to internal thread details and also synchronization. One could write a generic function whose sole purpose is to simply set the ``.kind`` field on every structure properly, and then pass that to an underlying normal function, but it is again a serious amount of effort for a dubious amount of gain.



Function Pointer-Style
----------------------

Another popular suggestion is to use a function pointer. Given the nature of the problem space -- namely, that once a thread is created it is usually immediately ran (at least in POSIX threads, Win32 threads can be created in a suspended state) -- this approach becomes incredibly difficult to work with in a way that's not painful.

``thrd_t`` and its various incarnations on many platforms is not a thing that is simple to interface with. The person who receives a ``thrd_t`` -- even with a special ``implementaioin-defined thrd_get_native_handle(thrd_t thr)`` function also present -- still has to work through the various awful struggles for each and every platform. There's no less than 5 ways to set the thread name on POSIX-conformant platforms with POSIX threads, all of them non-portable, with different implementation behaviors, and different allowable points of invocation (some can do it outside the thread, some can only do it inside of the thread whose name needs to be changed).

A way of taming that is developing a ``int (thrd_t* thr, void* userdata);`` function that gets paired with a enumeration "invocation site". One would use the "invocation site" enumeration value to determine whether to invoke on the newly created thread, or the old, requesting thread. Finally, the function would need the power to shut down thread creation by returning a value that isn't ``thrd_success``, much like the :cpp:type:`ztdc_thrd_attr_err_func_t`. It might end up looking like so:

.. code-block:: c
	:linenos:

	typedef int(thrd_attr_func_t)(thrd_t* thr, void* userdata);

	typedef enum thrd_invoke_loc {
		thrd_invoke_loc_requesting_thread,
		thrd_invoke_loc_new_thread,
	} thrd_invoke_loc;

	typedef struct thrd_attr_func {
		thrd_invoke_loc location;
		thrd_attr_func_t* attr_func;
		void* userdata;
	} thrd_attr_func;

However, it's easy to see that this structure, is really just missing one member missing from being a normal ``ztdc_thrd_attr_*`` style structure:

.. code-block:: c
	:linenos:

	typedef struct thrd_attr_func {
		ztdc_thrd_attr_kind kind; // ta-da!
		thrd_invoke_loc location;
		thrd_attr_func_t* attr_func;
		void* userdata;
	} thrd_attr_func;


This once again speaks to the raw flexibility of the chosen design, that this can be added as part of this design later. It's not added as one right now because that would, technically, add for some extra synchronization coordination particularly for the Win32 implementation. It's not too hard, so its likely on the table for a further release. Another thing to consider is that would need to be considered for the use of this function, though, is synchronization not just from where the function call is done from, but in the access of the ``userdata`` may need to be protected carefully if its being invoked on different threads; nominally, one may need to pass barrier or mutex data inside of whatever user data comes along with that pointer as well. Finally, any thread local variables made will need to be carefully considered in the context of ``thrd_invoke_loc``.



Simply providing ``native_handle()`` and ``native_id()`` functions?
-------------------------------------------------------------------

This library already does provide :cpp:func:`ztdc_thrd_get_native_handle` and :cpp:func:`ztdc_thrd_get_id` functions. But, as stated in many other parts of this library, it is insufficient for many platforms. Things like stack size, stack commit area, guard pages, and much more are decided on creation of many thread implementations. Some threads do not even offer a way to set or get the name of a thread once it's created. This limits the usefulness of creating a thread and *then* invoking :cpp:func:`ztdc_thrd_get_native_handle` or :cpp:func:`ztdc_thrd_get_id`.
