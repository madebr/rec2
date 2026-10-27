# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2022-2026 rec2 developers

add_library(IForce2::IForce2 SHARED IMPORTED GLOBAL)
set_property(TARGET IForce2::IForce2 PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
set_property(TARGET IForce2::IForce2 PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/x86/iforce2.lib")
