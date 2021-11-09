#
# Module for finding FreeImagePlus
#
# The module will define:
# FreeImagePlus_FOUND - True if FreeImagePlus development files were found
# FREEIMAGEPLUS_INCLUDE_DIR - FreeImagePlus include directories
# FREEIMAGEPLUS_LIBRARY - FreeImagePlus libraries to link
#
# FreeImagePlus target will be created for cmake 3.0.0 and newer
#


if(WIN32)

    find_path(FREEIMAGEPLUS_INCLUDE_DIR FreeImagePlus.h
        ${FREEIMAGEPLUS_ROOT_DIR}/include
        ${FREEIMAGEPLUS_ROOT_DIR}
        DOC "The directory where FreeImagePlus.h resides")

    find_library(FREEIMAGEPLUS_LIBRARY
        NAMES FreeImagePlus freeimageplus
        PATHS
        ${FREEIMAGEPLUS_ROOT_DIR}/lib
        ${FREEIMAGEPLUS_ROOT_DIR}
        DOC "The FreeImagePlus library")

else()

    find_path(FREEIMAGEPLUS_INCLUDE_DIR FreeImagePlus.h
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        DOC "The directory where FreeImagePlus.h resides")

    find_library(FREEIMAGEPLUS_LIBRARY
    	NAMES FreeImagePlus freeimageplus
    	PATHS
    	/usr/lib64
    	/usr/lib
    	/usr/local/lib64
    	/usr/local/lib
    	/sw/lib
    	/opt/local/lib
    	DOC "The FreeImagePlus library")

endif()

# set *_FOUND flag
if(FREEIMAGEPLUS_INCLUDE_DIR AND FREEIMAGEPLUS_LIBRARY)
   set(FREEIMAGEPLUS_FOUND True)
endif()


MARK_AS_ADVANCED(
        FREEIMAGEPLUS_FOUND 
        FREEIMAGEPLUS_LIBRARY
        FREEIMAGEPLUS_INCLUDE_DIR)
