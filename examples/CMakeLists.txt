# =============================================================================
#
# ztd.thread
# Copyright © JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
# Contact: opensource@soasis.org
#
# Commercial License Usage
# Licensees holding valid commercial ztd.thread licenses may use this file in
# accordance with the commercial license agreement provided with the
# Software or, alternatively, in accordance with the terms contained in
# a written agreement between you and Shepherd's Oasis, LLC.
# For licensing terms and conditions see your agreement. For
# further information contact opensource@soasis.org.
#
# Apache License Version 2 Usage
# Alternatively, this file may be used under the terms of Apache License
# Version 2.0 (the "License") for non-commercial use; you may not use this
# file except in compliance with the License. You may obtain a copy of the
# License at
#
# https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ============================================================================>

file(GLOB_RECURSE ztd.thread.examples.sources
	LIST_DIRECTORIES OFF
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source
	CONFIGURE_DEPENDS
	source/*.cpp source/*.c
)

foreach (example_source_name ${ztd.thread.examples.sources})
	set(example_source_file source/${example_source_name})
	set(example_target ztd.thread.examples.${example_source_name})
	add_executable(${example_target} ${example_source_file})
	target_compile_options(${example_target}
		PRIVATE
		${--utf8-literal-encoding}
		${--utf8-source-encoding}
		${--disable-permissive}
		${--warn-pedantic}
		${--warn-all}
		${--warn-extra}
		${--warn-errors}
	)
	
	target_link_libraries(${example_target}
		PRIVATE
		ztd::thread
		${CMAKE_DL_LIBS}
	)
	target_include_directories(${example_target}
		PRIVATE
		../include
	)
	if (ZTD_THREAD_TESTS)
		add_test(NAME ${example_target} COMMAND ${example_target})
	endif()
endforeach()
