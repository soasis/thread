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

Core Design
===========

The design of this library is centered around 4 things:

- the :cpp:enum:`ztdc_thrd_attr_kind` enumeration having appropriate values that map to a specific structure;
- the :cpp:struct:`ztdc_thrd_attr_* <ztdc_thrd_attr_name>` structures (and the :cpp:struct:`ztdc_thrd_attr__* implementation-defined structures <ztdc_thrd_attr__stack_storage>`) for each enumerator in the :cpp:enum:`ztdc_thrd_attr_kind` enumeration;
- the :cpp:func:`ztdc_thrd_create_attrs` and :cpp:func:`ztdc_thrd_create_attrs_err` functions allowing an implementation to set data before or immediately after thread creation;
- and, the :cpp:type:`ztdc_thrd_attr_err_func` error function type used in :cpp:func:`ztdc_thrd_create_attrs_err` to allow for checking and either passing over or denying an attribute.

Starting from the beginning, it is necessary to talk about the enumeration and the structures and the extensible, ABI-resistant nature of the interface. Then, we have to address what happens if these extensible, implementation-defined attributes are not recognized by an implementation.



Extensibility and Future-Proofed API
------------------------------------

One of the biggest issues uncovered in previous attempts to fix this problem in C (`N2419 <https://www.open-std.org/JTC1/SC22/WG14/www/docs/n2419.htm>`_) and C++ (`P0320 <https://wg21.link/P0320>`_, `P2019 <https://wg21.link/p2019>`_, `P3072 <https://wg21.link/p3072>`_, and `P3022 <https://wg21.link/p3022>`_) was the lack of extensibility and the lack of future-proofing against ABI issues. At the Minneapolis 2024 WG14 meeting, a vote was taken to allow undefined behavior for ``strftime`` and the broken-down time structures (see `N3272, Option 1 <https://www.open-std.org/JTC1/SC22/WG14/www/docs/n3272.htm>`_) due to the fact that it would be an ABI break in some places where users were directly copying structures. Our current interfaces do not have a great track record when they need to be updated, upgraded, or maintained.

Therefore, there is a need for flexibility in an interface which we expect to change over time.

One way to do this is with Windows-style ``Ex`` suffixes on functions and structures, alongside some built-in checking by having the first or last member of a structure be a `cbSize` member meant to be filled in by the `sizeof(...)` of the structure it's contained within. However, a good portion of the C community (Linux and BSD-derivates, in particular) would likely riot over such a style of interface, even if it is common place in Win32-style APIs. Unfortunately, Linux's and BSD's typical approach to this problem is to simply write the direct struct and then hem & haw for decades over improvements, providing entirely new functions with new structures or just shrugging their shoulders and leaving every individual vendor and user to fend for themselves when the day comes that new interfaces are needed.

A way to resolve these concerns is to combine the slimness of the Linux API but the ability to have multiple structures like the Win32 APIs but without creating new entry points. But, that poses an additional problem: how do we keep a unified interface that can handle multiple different structures (multiple different thread attributes) now and into the future without getting caught on ABI issues that force us to make such improvements Undefined Behavior, as in `N3272 <https://www.open-std.org/JTC1/SC22/WG14/www/docs/n3272.htm>`_?


Solution: Using Tags
++++++++++++++++++++

The solution for this is to recognize 2 different important qualities we need:

- we need a tag to identify a structure for both a user and an implementation to recognize a given thread attribute;
- and, we need a standards-compliant way of taking what will probably be a type-erased pointer to a structure and casting to the right thread attribute.

This would bring us to the first design, a ``void*`` plus ``attr_kind`` sort of tagging system:

.. code-block:: c
	:linenos:

	#include <stddef.h>
	#include <assert.h>

	typedef enum attr_kind {
		attr_kind_a,
		attr_kind_b,
	} attr_kind;

	struct attr_a {
		size_t stack_size;
	};

	struct attr_b {
		size_t guard_pages;
	};

	typedef struct tag_and_attr {
		attr_kind tag;
		void* attribute;
	} tag_and_attr;

	enum {
		do_some_success,
		do_some_fail,
		do_some_busy,
		do_some_memory,
		do_some_on_fire,
	};

	int do_something (size_t n, tag_and_attr attrs[]);

	int main () {
		attr_a a = { 0 };
		attr_b b = { 0 };
		tag_and_attr attrs[] = {
			{ .tag = attr_kind_a, &attr_a },
			{ .tag = attr_kind_b, &attr_b },
		};

		int err = do_something(2, attrs);
		assert(err == do_some_success);
		// ...

		return 0;
	}

One would then take a ``tag_and_attr[]`` -- an array of such objects -- to denote which actions to take. But this has obvious disadvantages native to C as a language: it is completely type unsafe. ``void*`` can point to literally anything. This isn't helpful, and makes the interface less compelling and competitive against a C++-style implementation which uses templates to not only not pay for any extra code but can treat each attribute completely separately and properly. This is where a rule in C and C++ comes in to save us:

	A pointer to a structure object, suitably converted, points to its initial member (or if that member is a bit-field, then to the unit in which it resides), and vice versa. There can be unnamed padding within a structure object, but not at its beginning.

We can (slightly) improve type safety by noting that, so long as we have a ``attr_foo`` structure, if the first member is an ``attr_kind`` type, a pointer to that ``attr_kind`` will also serve as the attribute structure's overall pointer. That means we can flatten our ``tag_and_attr`` structure to save on a very tiny bit of space, while improving type safety by putting the attribute kind into the structure itself rather than as a separate piece:

.. code-block:: c
	:linenos:

	#include <stddef.h>
	#include <assert.h>

	typedef enum attr_kind {
		attr_kind_a,
		attr_kind_b,
	} attr_kind;

	struct attr_a {
		attr_kind kind;
		size_t stack_size;
	};

	struct attr_b {
		attr_kind kind;
		size_t guard_pages;
	};

	enum {
		do_some_success,
		do_some_fail,
		do_some_busy,
		do_some_memory,
		do_some_on_fire,
	};

	int do_something (size_t n, attr_kind* attrs[]);

	int main () {
		attr_a a = { attr_kind_a, 0 };
		attr_b b = { attr_kind_b, 0 };
		attr_kind* attrs[] = {
			{ &a.kind },
			{ &b.kind },
		};

		int err = do_something(2, attrs);
		assert(err == do_some_success);

		// ...

		return 0;
	}

This saves us having to specify the ``kind`` with every entry and moves it instead to the initialization of the attribute itself. It also makes it so rather than taking a pointer to *literally anything*, it is instead focused on a narrow and specific set of values. Safety can further be increased with a set of creation functions that can't make the mistake of providing the wrong ``attr_kind`` and hardocding it in. (C++ would solve this problem by baking it into the type's constructor.) Because of the rule about the first element of a struct having an identical address as the whole structure, the following becomes the proper way to cast from each ``attr_kind`` to its respective attribute type:

.. code-block:: c
	:linenos:

	int do_something (size_t n, attr_kind* attrs[]) {
		for (size_t i = 0; i < n; ++i) {
			attr_kind* attr_tag = attrs[i];
			if (!attr_tag) {
				continue;
			}
			switch (*attr_tag) {
				case attr_kind_a: {
					attr_a* attr = (attr_a*)(void*)attr_tag;
					// use attr-> to get information about an attribute a
				} break;
				case attr_kind_b: {
					attr_b* attr = (attr_b*)(void*)attr_tag;
					// use attr-> to get information about an attribute b
				} break;
				default:
					break;
			}
		}

		// ...
		return do_some_success;
	}

This forms the basis for :cpp:func:`ztdc_thrd_create_attrs`. Unknown attributes that are not recognized by ``do_something`` (i.e., by :cpp:func:`ztdc_thrd_create_attrs`) are simply passed over. The others are processed in the function and the implementation can react to them. This allows for implementations to accept more structures in the future, so long as a new tag can be defined. The enumeration's underlying type is ``int`` right now, and so on typical implementations this can hold about 2 billion values (if they're all positive values). There's no way either the standard or implementations will ever fill all 2 billion of those values and have a structure to go along with it.



"What if an attribute is unrecognized / errors?"
------------------------------------------------

The second problem arises after we finally get an interface that allows for implementation-defined and standard-defined thread attributes: what happens if an implementation cannot honor an attribute? There's 2 ways that such failures happen:

- the thread attribute is provided by the user by the implementation does not recognize it;
- and, the thread attribute is provided by the user, the implementation recognizes it, but it cannot honor it for one reason or another.

This is surprisingly more common than one would believe for things like thread attributes. To give a more practical example, :cpp:struct:`ztdc_thrd_attr_stack_size` on Win32 Thread-based platforms with a too-small ``size`` request will just round the size up to the minimum size. But a too-small request on a POSIX thread-based platform will actually error and ignore the request with that specific stack size. The difference here matters because different platforms have wildly different implementation strategies, and it is impossible to provide guaranteed minimums to a unified interface in a way that's helpful.

Therefore, the more user-friendly thing to do is to allow a user to know which attributes are not honored by the implementation. This means that our ``do_something`` function from before needs to take a function (and a userdata parameter, as is typical for user-controlled APIs). So, let's add that to the interface:

.. code-block:: c
	:linenos:

	#include <stddef.h>
	#include <assert.h>
	#include <stdio.h>

	typedef enum attr_kind {
		attr_kind_a,
		attr_kind_b,
	} attr_kind;

	struct attr_a {
		attr_kind kind;
		size_t stack_size;
	};

	struct attr_b {
		attr_kind kind;
		size_t guard_pages;
	};

	enum {
		do_some_success,
		do_some_fail,
		do_some_busy,
		do_some_memory,
		do_some_on_fire,
	};

	typedef int(do_some_err_func_t)(attr_kind* attr, int err, void* userdata);

	int do_something (size_t n, attr_kind* attrs[]);
	int do_something_err (size_t n, attr_kind* attrs[],
		do_some_err_func_t* func_err, void* func_err_userdata);

	int check_attr(attr_kind* attr, int err, void* func_err_userdata) {
		if (*attr == attr_kind_a) {
			fprintf(stderr, "stack size cannot be honored, "
				"but we will continue anyways: %zu",
				((attr_a*)attr)->stack_size);
			return err;
			
		}
		if (*attr == attr_kind_b) {
			fprintf(stderr, "guard page size cannot be honored, "
				"but we will continue anyways: %zu",
				((attr_b*)attr)->guard_pages);
			return do_some_success;
		}
		fprintf(stderr, "unknown attribute: %d "
			"(we will continue anyways)", (unsigned int)*attr);
		return do_some_success;
	}

	int main () {
		attr_a a = { attr_kind_a, 0 };
		attr_b b = { attr_kind_b, 0 };
		typedef struct attr_c = {
			attr_kind kind;
			const char* name;
		} attr_c;
		attr_c c = { 359503, "teehee" };
		attr_kind* attrs[] = {
			{ &a.kind },
			{ &b.kind },
			{ &c.kind },
		};

		int err = do_something_err(2, attrs, check_attr, NULL);
		assert(err == do_some_success);

		// ...

		return 0;
	}

Here, we have a ``check_attr`` function that gives us the appropriate ability to inspect and check the values passed to the function. We can report errors as we please, without needing to inspect the internals of the ``do_something`` implementation. The implementation ultimately controls what it does and does not know about, however:

.. code-block:: c
	:linenos:

	#include <stddef.h>

	typedef struct secret_attr {
		attr_kind kind;
		const char* name;
		size_t size;
	} secret_attr;

	int do_something_err(size_t n, attr_kind* attrs[],
		do_some_err_func_t* func_err, void* func_err_userdata)
	{
		for (size_t i = 0; i < n; ++i) {
			attr_kind* attr_tag = attrs[i];
			if (!attr_tag) {
				continue;
			}
			switch (*attr_tag) {
				case attr_kind_a: {
					attr_a* attr = (attr_a*)(void*)attr_tag;
					// we can handle this one, no need to call the error!
					// ...
				} break;
				case attr_kind_b: {
					attr_b* attr = (attr_b*)(void*)attr_tag;
					// we don't know what to do for this one, etc. etc.
					// ...
					int attr_err = func_err(attr_kind,
					                        do_some_fail,
					                        func_err_userdata);
					if (attr_err != do_some_success) {
						// do not proceed: leave
						return attr_err;
					}
				} break;
				case 0x10000: {
					secret_attr* attr = (secret_attr*)(void*)attr_tag;
					// secret implementation-defined attribute this
					// specific implementation knows about
					// ...
				} break;
				default: {
					// we do not recognize the attribute at ALL
					int attr_err = func_err(attr_kind,
					                        do_some_fail,
					                        func_err_userdata);
					if (attr_err != do_some_success) {
						// do not proceed: leave
						return attr_err;
					}
				} break;
			}
		}

		// ...
		return do_some_success;
	}

If the ``func_err`` is called and it returns something other than ``do_some_success``, then we know that the implementation could not process the attribute. This allows the implementation to control what it wants to handle, but also lets the user report / crash / etc. on any kind of failure to handle an attribute. This is the driving force behind :cpp:func:`ztdc_thrd_create_attrs_err`, and forms the core of the additions to this API.



Achieving the Goal
------------------

There are some additional constraints and workarounds for specific implementation shenanigans and the fact that we're not just writing purely synchronous code here. But this is the core principles behind how ``ztdc_thrd_create_attrs_err`` and how it's structured works. At any point, an implementation can add a new ``secret_attr`` that it wants to work with, and it won't disturb the other structures or older implementations. And, older implementations that are called with newer attributes will simply just report them to the user.

By default, ``do_something`` (and its analogous :cpp:func:`ztdc_thrd_create_attrs`) take the approach of using a function that simply returns ``do_some_success`` for an error as by-default one wants to ignore any weird implementation quirks:

.. code-block:: cpp
	:linenos:

	int default_do_something_err(attr_kind* attr, int err, void* userdata) {
		return do_some_success;
	}

	int do_something(size_t n, attr_kind* attrs[],
		do_some_err_func_t* func_err, void* func_err_userdata)
	{
		return do_something_err(n, attrs, default_do_something_err, NULL);
	}

	int do_something_err(size_t n, attr_kind* attrs[],
		do_some_err_func_t* func_err, void* func_err_userdata)
	{
		// implementation from above...
		// ...
	}

This is the full core of the design of ``ztd.thread``.
