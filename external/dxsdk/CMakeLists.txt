add_library(DirectX::dxguid STATIC IMPORTED GLOBAL)
set_property(TARGET DirectX::dxguid PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/dxsdk/lib/dxguid.lib")

add_library(DirectX::dinput SHARED IMPORTED GLOBAL)
set_property(TARGET DirectX::dinput PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/dxsdk/lib/dinput.lib")
if(REC2_MATCHING)
    set_property(TARGET DirectX::dinput PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/dxsdk/include")
endif()

add_library(DirectX::d3dim SHARED IMPORTED GLOBAL)
set_property(TARGET DirectX::d3dim PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/dxsdk/lib/d3dim.lib")
if(REC2_MATCHING)
    set_property(TARGET DirectX::d3dim PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/dxsdk/include")
endif()
