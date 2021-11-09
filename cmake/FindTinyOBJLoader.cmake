#
# Module for finding TinyOBJLoader
#
# The module will define:
# TINYOBJLOADER_FOUND - True if TinyOBJLoader development files were found
# TINYOBJLOADER_INCLUDE_DIR - TinyOBJLoader include directories
# TINYOBJLOADER_LIBRARY - TinyOBJLoader libraries to link
#
# TinyOBJLoader target will be created for cmake 3.0.0 and newer
#


if(WIN32)

    find_path(TINYOBJLOADER_INCLUDE_DIR tiny_obj_loader.h
        ${FREEIMAGEPLUS_ROOT_DIR}/include
        ${FREEIMAGEPLUS_ROOT_DIR}
        DOC "The directory where tiny_obj_loader.h resides")

    find_library(TINYOBJLOADER_LIBRARY
        NAMES TINYOBJLOADER tinyobjloader
        PATHS
        ${FREEIMAGEPLUS_ROOT_DIR}/lib
        ${FREEIMAGEPLUS_ROOT_DIR}
        DOC "The TinyOBJLoader library")

else()

    find_path(TINYOBJLOADER_INCLUDE_DIR tiny_obj_loader.h
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        DOC "The directory where tiny_obj_loader.h resides")

    find_library(TINYOBJLOADER_LIBRARY
        NAMES TINYOBJLOADER tinyobjloader
        PATHS
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
        DOC "The TinyOBJLoader library")

endif()

# set *_FOUND flag
if(TINYOBJLOADER_INCLUDE_DIR AND TINYOBJLOADER_LIBRARY)
   set(TINYOBJLOADER_FOUND True)
endif()


MARK_AS_ADVANCED(
        TINYOBJLOADER_FOUND 
        TINYOBJLOADER_LIBRARY
        TINYOBJLOADER_INCLUDE_DIR)
 
