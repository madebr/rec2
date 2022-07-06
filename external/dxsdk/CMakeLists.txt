add_library(DirectX::dxguid STATIC IMPORTED GLOBAL)
set_property(TARGET DirectX::dxguid PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/dxsdk/lib/dxguid.lib")

add_library(DirectX::dinput SHARED IMPORTED GLOBAL)
set_property(TARGET DirectX::dinput PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/dxsdk/lib/dinput.lib")
