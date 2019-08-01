#.rst:
# FindxxHash
# -----------
#
# Find the xxHash library header and define variables.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines :prop_tgt:`IMPORTED` target ``xxHash::xxHash``,
# if xxHash has been found
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   XXHASH_FOUND          - True if xxHash is found.
#   XXHASH_INCLUDE_DIRS   - Where to find xxhash.h
#   XXHASH_LIBRARIES      - Where to find xxhash library
#
# ::
#
#   XXHASH_VERSION        - The version of xxHash found (x.y.z)
#   XXHASH_VERSION_MAJOR  - The major version of xxHash
#   XXHASH_VERSION_MINOR  - The minor version of xxHash
#   XXHASH_VERSION_PATCH  - The patch version of xxHash

find_path(XXHASH_INCLUDE_DIR NAME xxhash.h PATH_SUFFIXES include)
find_library(XXHASH_LIBRARY NAMES xxhash xxHash PATH_SUFFIXES lib)

mark_as_advanced(XXHASH_INCLUDE_DIR)

if(XXHASH_INCLUDE_DIR AND EXISTS "${XXHASH_INCLUDE_DIR}/xxhash.h")
  file(STRINGS "${XXHASH_INCLUDE_DIR}/xxhash.h" XXHASH_H REGEX "^#define XXH_VERSION_[A-Z]+[ ]+[0-9]+$")
  string(REGEX REPLACE ".+XXH_VERSION_MAJOR[ ]+([0-9]+).*$"   "\\1" XXHASH_VERSION_MAJOR "${XXHASH_H}")
  string(REGEX REPLACE ".+XXH_VERSION_MINOR[ ]+([0-9]+).*$"   "\\1" XXHASH_VERSION_MINOR "${XXHASH_H}")
  string(REGEX REPLACE ".+XXH_VERSION_RELEASE[ ]+([0-9]+).*$" "\\1" XXHASH_VERSION_PATCH "${XXHASH_H}")
  set(XXHASH_VERSION "${XXHASH_VERSION_MAJOR}.${XXHASH_VERSION_MINOR}.${XXHASH_VERSION_PATCH}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(xxHash
  REQUIRED_VARS XXHASH_LIBRARY XXHASH_INCLUDE_DIR VERSION_VAR XXHASH_VERSION)

if(XXHASH_FOUND)
  set(XXHASH_INCLUDE_DIRS "${XXHASH_INCLUDE_DIR}")

  if(NOT XXHASH_LIBRARIES)
    set(XXHASH_LIBRARIES ${XXHASH_LIBRARY})
  endif()

  if(NOT TARGET xxHash::xxHash)
    add_library(xxHash::xxHash UNKNOWN IMPORTED)
    set_target_properties(xxHash::xxHash PROPERTIES
      IMPORTED_LOCATION "${XXHASH_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${XXHASH_INCLUDE_DIRS}")
  endif()
endif()
