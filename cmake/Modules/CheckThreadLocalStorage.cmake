# Copyright 2012 Intel Corporation
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
