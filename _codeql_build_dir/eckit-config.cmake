# Config file for the eckit package
# Defines the following variables:
#
#  eckit_FEATURES       - list of enabled features
#  eckit_VERSION        - version of the package
#  eckit_GIT_SHA1       - Git revision of the package
#  eckit_GIT_SHA1_SHORT - short Git revision of the package
#


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was project-config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

### computed paths
set_and_check(eckit_CMAKE_DIR "${PACKAGE_PREFIX_DIR}/.")
set_and_check(eckit_BASE_DIR "${PACKAGE_PREFIX_DIR}/")
if(DEFINED ECBUILD_2_COMPAT AND ECBUILD_2_COMPAT)
  set(ECKIT_CMAKE_DIR ${eckit_CMAKE_DIR})
  set(ECKIT_BASE_DIR ${eckit_BASE_DIR})
endif()

### export version info
set(eckit_VERSION           "2.0.0")
set(eckit_GIT_SHA1          "565b89d1d03f392b4b7aba18c95cfe2eb25f503f")
set(eckit_GIT_SHA1_SHORT    "565b89d")

if(DEFINED ECBUILD_2_COMPAT AND ECBUILD_2_COMPAT)
  set(ECKIT_VERSION           "2.0.0" )
  set(ECKIT_GIT_SHA1          "565b89d1d03f392b4b7aba18c95cfe2eb25f503f" )
  set(ECKIT_GIT_SHA1_SHORT    "565b89d" )
endif()

### has this configuration been exported from a build tree?
set(eckit_IS_BUILD_DIR_EXPORT ON)
if(DEFINED ECBUILD_2_COMPAT AND ECBUILD_2_COMPAT)
  set(ECKIT_IS_BUILD_DIR_EXPORT ${eckit_IS_BUILD_DIR_EXPORT})
endif()

### include the <project>-import.cmake file if there is one
if(EXISTS ${eckit_CMAKE_DIR}/eckit-import.cmake)
  set(eckit_IMPORT_FILE "${eckit_CMAKE_DIR}/eckit-import.cmake")
  include(${eckit_IMPORT_FILE})
endif()

### insert definitions for IMPORTED targets
if(NOT eckit_BINARY_DIR)
  find_file(eckit_TARGETS_FILE
    NAMES eckit-targets.cmake
    HINTS ${eckit_CMAKE_DIR};/home/runner/work/eckit/eckit/_codeql_build_dir
    NO_DEFAULT_PATH)
  if(eckit_TARGETS_FILE)
    include(${eckit_TARGETS_FILE})
  endif()
endif()

### include the <project>-post-import.cmake file if there is one
if(EXISTS ${eckit_CMAKE_DIR}/eckit-post-import.cmake)
  set(eckit_POST_IMPORT_FILE "${eckit_CMAKE_DIR}/eckit-post-import.cmake")
  include(${eckit_POST_IMPORT_FILE})
endif()

### handle third-party dependencies
if(DEFINED ECBUILD_2_COMPAT AND ECBUILD_2_COMPAT)
  set(ECKIT_LIBRARIES         "")
  set(ECKIT_TPLS              "" )

  include(${CMAKE_CURRENT_LIST_FILE}.tpls OPTIONAL)
endif()

### publish this file as imported
if( DEFINED ECBUILD_2_COMPAT AND ECBUILD_2_COMPAT )
  set(eckit_IMPORT_FILE ${CMAKE_CURRENT_LIST_FILE})
  mark_as_advanced(eckit_IMPORT_FILE)
  set(ECKIT_IMPORT_FILE ${CMAKE_CURRENT_LIST_FILE})
  mark_as_advanced(ECKIT_IMPORT_FILE)
endif()

### export features and check requirements
set(eckit_FEATURES "TESTS;PKGCONFIG;BUILD_TOOLS;ECKIT_CMD;ECKIT_SQL;LZ4;XXHASH;ECKIT_CODEC;ECKIT_SPEC;ECKIT_GEO;UNICODE;AIO;WARNINGS;WARNINGS")
if(DEFINED ECBUILD_2_COMPAT AND ECBUILD_2_COMPAT)
  set(ECKIT_FEATURES ${eckit_FEATURES})
endif()
foreach(_f ${eckit_FEATURES})
  set(eckit_${_f}_FOUND 1)
  set(eckit_HAVE_${_f} 1)
  if(DEFINED ECBUILD_2_COMPAT AND ECBUILD_2_COMPAT)
    set(ECKIT_HAVE_${_f} 1)
  endif()
endforeach()
check_required_components(eckit)
