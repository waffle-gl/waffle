# Copyright 2013 Intel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set(cmocka_source_dir ${CMAKE_SOURCE_DIR}/third_party/cmocka)
set(cmocka_build_dir ${CMAKE_BINARY_DIR}/third_party/cmocka/build)
set(cmocka_cmake_output_file ${cmocka_build_dir}/waffle-cmake.log)
set(cmocka_cmake_error_file ${cmocka_build_dir}/waffle-cmake.err)
set(cmocka_cmake_command "cd \"${cmocka_build_dir}\" && cmake -G\"${CMAKE_GENERATOR}\" -DCMAKE_C_COMPILER=\"${CMAKE_C_COMPILER}\" \"${cmocka_source_dir}\"")

if(waffle_on_linux)
    set(cmocka_library_location ${cmocka_build_dir}/src/libcmocka.so)
elseif(waffle_on_mac)
    set(cmocka_library_location ${cmocka_build_dir}/src/libcmocka.dylib)
endif()

include_directories(${cmocka_source_dir}/include)
link_directories(${cmocka_build_dir}/src)

add_library(cmocka SHARED IMPORTED)
set_target_properties(cmocka
    PROPERTIES
        IMPORTED_LOCATION ${cmocka_library_location}
)
add_dependencies(cmocka cmocka-build)
add_custom_command(
    OUTPUT ${cmocka_library_location}
    WORKING_DIRECTORY ${cmocka_build_dir}
    COMMAND cmake --build .
)

add_custom_target(cmocka-build
    DEPENDS ${cmocka_library_location}
)
add_custom_target(cmocka-clean
    WORKING_DIRECTORY ${cmocka_build_dir}
    COMMAND cmake --build . --target clean
)

# Configure CMocka.
if(NOT EXISTS ${cmocka_build_dir}/CMakeCache.txt)
    file(MAKE_DIRECTORY ${cmocka_build_dir})
    message(STATUS "Configuring subproject third_party/cmocka")
    message(STATUS "${cmocka_cmake_command}")
    execute_process(
        COMMAND sh -c ${cmocka_cmake_command}
        RESULT_VARIABLE cmocka_cmake_result
        OUTPUT_FILE ${cmocka_cmake_output_file}
        ERROR_FILE ${cmocka_cmake_error_file}
    )
    if(NOT ${cmocka_cmake_result} EQUAL 0)
        if(EXISTS ${cmocka_cmake_error_file})
            message(STATUS "Error messages from subproject third_party/cmocka:")
            message(STATUS)
            execute_process(COMMAND cat ${cmocka_cmake_error_file})
        endif()
        message(STATUS "See following files for details on cmocka configuration failure::")
        message(STATUS "  ${cmocka_cmake_output_file}")
        message(STATUS "  ${cmocka_cmake_error_file}")
        message(FATAL_ERROR "Failed to configure subproject third_party/cmocka")
    endif()
endif()
