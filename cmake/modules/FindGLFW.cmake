# From the LearnOpenGL github repo

set( _glfw_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/includes" )

set( _glfw_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib" )

# Check environment for root search directory
set( _glfw3_ENV_ROOT $ENV{GLFW3_ROOT} )

if( NOT GLFW3_ROOT AND _glfw3_ENV_ROOT )
	set(GLFW3_ROOT ${_glfw3_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( GLFW3_ROOT )
	list( INSERT _glfw_HEADER_SEARCH_DIRS 0 "${GLFW3_ROOT}/include" )
	list( INSERT _glfw_LIB_SEARCH_DIRS 0 "${GLFW3_ROOT}/lib" )
endif()

# Search for the header
find_path(GLFW_INCLUDE_DIR "GLFW/glfw3.h"
PATHS ${_glfw_HEADER_SEARCH_DIRS} )

# Search for the library
Find_library(GLFW_LIBRARY NAMES glfw3 glfw
PATHS ${_glfw_LIB_SEARCH_DIRS} )
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW DEFAULT_MSG
GLFW_LIBRARY GLFW_INCLUDE_DIR)
