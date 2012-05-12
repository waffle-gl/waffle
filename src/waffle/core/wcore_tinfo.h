// Copyright 2012 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// @defgroup wcore_tinfo wcore_tinfo
/// @ingroup wcore
///
/// @brief Thread-local info.
/// @{

/// @file

#pragma once

struct wcore_error_tinfo;

/// @brief Thread-local info for all of Waffle.
struct wcore_tinfo {
    /// @brief Info for @ref wcore_error.
    struct wcore_error_tinfo *error;
};

/// @brief Get the thread-local info for the current thread.
struct wcore_tinfo* wcore_tinfo_get(void);

/// @}
