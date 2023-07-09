add_executable(carma2 WIN32
    res/resources.rc
    src/global.c
    src/launcher.c
    src/main.c
    src/password.c
    src/util.c
)
target_include_directories(carma2 PRIVATE res)
target_link_libraries(carma2 dxguid)
