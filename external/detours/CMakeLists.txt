cmake_minimum_required(VERSION 3.0)
project(detours_vendored CXX)

file(GLOB DETOURS_SOURCES detours/src/*.cpp)
list(REMOVE_ITEM DETOURS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/detours/src/uimports.cpp"
)
file(GLOB DETOURS_HEADERS detours/src/*.h)
add_library(detours STATIC ${DETOURS_SOURCES} ${DETOURS_HEADERS})
target_include_directories(detours INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/detours/src>")
if(MINGW)
    target_compile_options(detours PRIVATE -w)
endif()

add_library(Detours::Detours ALIAS detours)
