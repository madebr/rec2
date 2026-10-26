# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

add_library(quicktime_dummy STATIC
    src/dummy.c
)
add_library(QuickTime::Dummy ALIAS quicktime_dummy)
target_include_directories(quicktime_dummy PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(quicktime_dummy PUBLIC c2_hooks)
