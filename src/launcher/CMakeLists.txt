# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

add_executable(carma2 WIN32
    res/resources.rc
    src/global.c
    src/launcher.c
    src/main.c
    src/password.c
    src/util.c
)
target_include_directories(carma2 PRIVATE res)
target_include_directories(carma2 PRIVATE $<TARGET_PROPERTY:DirectX::d3dim,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(carma2 DirectX::dxguid)
target_link_libraries(carma2 c2_hooks)

if(REC2_MATCHING)
endif()
