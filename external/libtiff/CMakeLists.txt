if(REC2_MATCHING)
    add_library(tiff STATIC
        libtiff-3.4/libtiff/tif_aux.c
        libtiff-3.4/libtiff/tif_close.c
        libtiff-3.4/libtiff/tif_codec.c
        libtiff-3.4/libtiff/tif_compress.c
        libtiff-3.4/libtiff/tif_dir.c
        libtiff-3.4/libtiff/tif_dirinfo.c
        libtiff-3.4/libtiff/tif_dirread.c
        libtiff-3.4/libtiff/tif_dirwrite.c
        libtiff-3.4/libtiff/tif_dumpmode.c
        libtiff-3.4/libtiff/tif_error.c
        libtiff-3.4/libtiff/tif_fax3.c
        libtiff-3.4/libtiff/tif_flush.c
        libtiff-3.4/libtiff/tif_getimage.c
        libtiff-3.4/libtiff/tif_jpeg.c
        libtiff-3.4/libtiff/tif_luv.c
        libtiff-3.4/libtiff/tif_lzw.c
        libtiff-3.4/libtiff/tif_next.c
        libtiff-3.4/libtiff/tif_open.c
        libtiff-3.4/libtiff/tif_packbits.c
        libtiff-3.4/libtiff/tif_pixarlog.c
        libtiff-3.4/libtiff/tif_predict.c
        libtiff-3.4/libtiff/tif_print.c
        libtiff-3.4/libtiff/tif_read.c
        libtiff-3.4/libtiff/tif_strip.c
        libtiff-3.4/libtiff/tif_swab.c
        libtiff-3.4/libtiff/tif_thunder.c
        libtiff-3.4/libtiff/tif_tile.c
        libtiff-3.4/libtiff/tif_version.c
        libtiff-3.4/libtiff/tif_warning.c
        libtiff-3.4/libtiff/tif_win32.c
        libtiff-3.4/libtiff/tif_write.c
        libtiff-3.4/libtiff/tif_zip.c
    )
    target_include_directories(tiff PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libtiff-3.4/libtiff")
else()
    set(tiff-tools OFF CACHE BOOL "" FORCE)
    set(tiff-tools OFF CACHE BOOL "" FORCE)
    set(tiff-tests OFF CACHE BOOL "" FORCE)
    set(tiff-contrib OFF CACHE BOOL "" FORCE)
    set(tiff-docs OFF CACHE BOOL "" FORCE)
    set(tiff-deprecated OFF CACHE BOOL "" FORCE)
    set(tiff-install OFF CACHE BOOL "" FORCE)

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(tiff-cxx OFF CACHE BOOL "" FORCE)
    set(fatal-warnings OFF CACHE BOOL "" FORCE)
    set(jbig OFF CACHE BOOL "" FORCE)
    set(jpeg OFF CACHE BOOL "" FORCE)
    set(jpeg12 OFF CACHE BOOL "" FORCE)
    set(lerc OFF CACHE BOOL "" FORCE)
    set(libdeflate OFF CACHE BOOL "" FORCE)
    set(lzma OFF CACHE BOOL "" FORCE)
    set(old-jpeg OFF CACHE BOOL "" FORCE)
    set(pixarlog OFF CACHE BOOL "" FORCE)
    set(webp OFF CACHE BOOL "" FORCE)
    set(zlib OFF CACHE BOOL "" FORCE)
    set(zstd OFF CACHE BOOL "" FORCE)

    add_subdirectory(libtiff EXCLUDE_FROM_ALL)
endif()
if(NOT TARGET TIFF::TIFF)
    add_library(TIFF::TIFF ALIAS tiff)
endif()

