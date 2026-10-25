# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

if(REC2_FIX_BUGS)
    add_definitions(-DREC2_FIX_BUGS)
endif()

add_subdirectory(hooks)
add_subdirectory(common)
add_subdirectory(newcommon)
add_subdirectory(s3)
add_subdirectory(platform)
