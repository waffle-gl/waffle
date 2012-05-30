# Copyright 2012 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# Summary:
#     Check if the compiler supports various types of thread-local storage.
#
# Usage:
#     include(CheckThreadLocalStorage)
#     check_thread_local_storage()
#
# Output Variables:
#     waffle_has_tls
#         True if the compiler supports the `__thread` attribute.
#
#     waffle_has_tls_model_initial_exec
#         True if the compiler supports `__attribute__(tls_model("initial-exec"))`.
#


include(CheckCSourceCompiles)

macro(check_thread_local_storage)
    # Clang, in its zeal for compatibility with gcc, emits only a warning if
    # it encounters an __attribute__ that it does not recognize. To prevent
    # this function from returning a false positive in that situation, force
    # check_c_source_compiles() to fail on warnings with FAIL_REGEX.

    check_c_source_compiles(
        "
        static __thread int x;

        int
        main() {
            x = 42;
            return x;
        }
        "

        waffle_has_tls

        FAIL_REGEX "warning|error"
        )

    check_c_source_compiles(
        "
        static __thread int x __attribute__((tls_model(\"initial-exec\")));

        int
        main() {
            x = 42;
            return x;
        }
        "

        waffle_has_tls_model_initial_exec

        FAIL_REGEX "warning|error"
        )
endmacro()
