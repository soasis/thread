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
.. =============================================================================

``ztd.thread``
==============

Welcome to the ``ztd.thread`` library, an experimental library for proving out the management of attributes in C code in a way that is drop-in compatible with C11 threads. Currently, there are shims for POSIX threads (so-called "pthreads") and Win32 threads. Either is compatible with ``<pthreads.h>`` on POSIX-based platforms, or ``<xthreads.h>`` on Windows platforms. It also works on such platforms even if it already has a C11 ``<threads.h>`` implemented, and simply works on top of that existing work.

There is currently no ``std::thread``/C++ integration, as it is much harder to crack open a ``std::thread`` type on the various platforms.



Who Is This Library For?
------------------------

Anyone who wants a better way to specify things like the name, or the stack size, of a thread. Additions and more improvements welcome within the ``ztdc_thrd_attr_*`` types and enumerations, implementation-specific or not!

.. toctree::
	:maxdepth: 1
	:caption: Contents:

	api
	design
	license




Indices & Search
================

.. toctree::

	genindex
