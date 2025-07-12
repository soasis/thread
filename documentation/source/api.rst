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

API
===

There's only a limited number of APIs to document, as the rest is covered by the C11 ``<threads.h>`` documentation. Therefore, we only document the new parts and the additions that this library specializes in.



``ztdc_thrd_native_handle_t`` type and functions
------------------------------------------------

.. doxygentypedef:: ztdc_thrd_native_handle_t

.. doxygenfunction:: ztdc_thrd_native_handle

.. doxygenfunction:: ztdc_thrd_current_native_handle

.. doxygenvariable:: ztdc_thrd_null_native_handle



``ztdc_thrd_id_t`` type and functions
-------------------------------------

.. doxygentypedef:: ztdc_thrd_id_t

.. doxygenfunction:: ztdc_thrd_id

.. doxygenfunction:: ztdc_thrd_current_id

.. doxygenvariable:: ztdc_thrd_null_id



``ztdc_thrd_attr_err_func_t`` function type and functions
---------------------------------------------------------

.. doxygentypedef:: ztdc_thrd_attr_err_func_t

.. doxygenfunction:: ztdc_thrd_ignore_all_attr_errors

.. doxygenfunction:: ztdc_thrd_stop_on_attr_error



``ztdc_thrd_create_attrs_err`` function
---------------------------------------

.. doxygenfunction:: ztdc_thrd_create_attrs_err



``ztdc_thrd_create_attrs`` function
-----------------------------------

.. doxygenfunction:: ztdc_thrd_create_attrs



``ztdc_thrd_attr_kind`` enumeration
-----------------------------------

This enumeration is the core of the library. Every standard

.. doxygenenum:: ztdc_thrd_attr_kind


.. _ztdc_thrd_attr:

``ztdc_thrd_attr_*`` standard structures
----------------------------------------

These are standard structures, useful to dictate behavior across platforms. Certain platforms have different limits on different parts of the attributes for threads: it may take some experimentation to make it work. Every structure has a :cpp:enum:`ztdc_thrd_attr_kind` as the first member in order for the enumerator's address to double as the address of the structure, and allow for well-defined casting to the appropriate type.

.. note::
	
	All attributes are processed on the thread that invoked one of the attribute-handling thread creation functions, except for :cpp:struct:`ztdc_thrd_attr_custom_on_new`, which is invoked on the new thread. :cpp:struct:`ztdc_thrd_attr_custom_on_new` and :cpp:struct:`ztdc_thrd_attr_custom_on_origin` are always invoked after all other attributes have been processed in `attrs` (and no errors have occurred). :cpp:struct:`ztdc_thrd_attr_custom_on_new` is processed before :cpp:struct:`ztdc_thrd_attr_custom_on_origin`, and none of their functions are ever invoked in parallel. The processing of all attributes synchronizes before the start of the actual passed-in thread function.

.. doxygenstruct:: ztdc_thrd_attr_name
	:members:

.. doxygenstruct:: ztdc_thrd_attr_name_sized
	:members:

.. doxygenstruct:: ztdc_thrd_attr_mcname
	:members:

.. doxygenstruct:: ztdc_thrd_attr_mcname_sized
	:members:

.. doxygenstruct:: ztdc_thrd_attr_mwcname
	:members:

.. doxygenstruct:: ztdc_thrd_attr_mwcname_sized
	:members:

.. doxygenstruct:: ztdc_thrd_attr_c8name
	:members:

.. doxygenstruct:: ztdc_thrd_attr_c8name_sized
	:members:

.. doxygenstruct:: ztdc_thrd_attr_c16name
	:members:

.. doxygenstruct:: ztdc_thrd_attr_c16name_sized
	:members:

.. doxygenstruct:: ztdc_thrd_attr_c32name
	:members:

.. doxygenstruct:: ztdc_thrd_attr_c32name_sized
	:members:

.. doxygenstruct:: ztdc_thrd_attr_stack_size
	:members:

.. doxygenstruct:: ztdc_thrd_attr_detached
	:members:

.. doxygenstruct:: ztdc_thrd_attr_custom_on_new
	:members:

.. doxygenstruct:: ztdc_thrd_attr_custom_on_origin
	:members:



.. _ztdc_thrd_attr_impl:

``ztdc_thrd_attr_*`` implementation-defined structures
------------------------------------------------------

These structures are less portable and do not work across platforms.

.. doxygenstruct:: ztdc_thrd_attr__stack_storage
	:members:

.. doxygenstruct:: ztdc_thrd_attr__stack_guard_size
	:members:



``ztdc_thrd_set_name`` functions
--------------------------------

.. doxygenfunction:: ztdc_thrd_set_native_name

.. doxygenfunction:: ztdc_thrd_set_mcname

.. doxygenfunction:: ztdc_thrd_set_mwcname

.. doxygenfunction:: ztdc_thrd_set_c8name

.. doxygenfunction:: ztdc_thrd_set_c16name

.. doxygenfunction:: ztdc_thrd_set_c32name



``ztdc_thrd_get_name`` functions
--------------------------------

.. doxygenfunction:: ztdc_thrd_get_native_name

.. doxygenfunction:: ztdc_thrd_get_mcname

.. doxygenfunction:: ztdc_thrd_get_mwcname

.. doxygenfunction:: ztdc_thrd_get_c8name

.. doxygenfunction:: ztdc_thrd_get_c16name

.. doxygenfunction:: ztdc_thrd_get_c32name



``ZTDC_THRD_*`` bounds macros
-----------------------------

.. doxygendefine:: ZTDC_THRD_MAXIMUM_NAME_SIZE

.. doxygendefine:: ZTDC_THRD_MINIMUM_STACK_SIZE

