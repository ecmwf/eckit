# file accessed in the test

file( WRITE "${CMAKE_BINARY_DIR}/etc/eckit/test/test.cfg" "Just for testing" )

set(_system_library_depends)
if(eckit_HAVE_ECKIT_CMD)
    list(APPEND _system_library_depends eckit_cmd)
endif()
if(eckit_HAVE_ECKIT_SQL)
    list(APPEND _system_library_depends eckit_sql)
endif()
if(eckit_HAVE_ECKIT_MPI)
    list(APPEND _system_library_depends eckit_mpi)
endif()

ecbuild_add_test(   TARGET      eckit_test_system_library
                    SOURCES     test_system_library.cc
                    LIBS        eckit
                    DEPENDS     ${_system_library_depends} )

#----------------------------------------------------------------------------------------------------------------------
# Test-only plugin shared libraries.
#
# Each entry below is a single CMake string of the form
#     <plugin-name>|<plugin-version>
# encoding the data that varies per test plugin. Every plugin is built from the
# single shared source plugin_template.cc, parameterised at compile time by the
# ECKIT_TEST_PLUGIN_NAME and ECKIT_TEST_PLUGIN_VERSION macros.
#
# Versions are intentionally varied to exercise every component of the semver
# triple (major / minor / patch) and a two-digit field, so a future bug in the
# version comparison or the manifest version-pin check is more likely to be
# caught by these tests.
#
# The libraries land in a dedicated build directory and are NOT installed nor
# linked into the test executables — they are dlopen'd via DYNAMIC_LIBRARY_PATH
# set per-test below, so the loading mechanism is genuinely exercised.
#----------------------------------------------------------------------------------------------------------------------

set(_plugin_specs
    "disabled-global-plugin|1.0.0"
    "disabled-scoped-plugin|1.2.0"
    "disable-test-enabled-scoped-plugin|1.2.3"
    "manifest-global-plugin|2.0.0"
    "manifest-scoped-plugin|2.5.0"
    "manifest-tagged-grids-plugin|3.0.1"
    "manifest-tagged-visual-plugin|3.4.0"
    "manifest-duplicate-plugin|4.0.0"
    "version-ok-plugin|5.0.0"
    "version-high-plugin|5.1.0"
    "version-missing-owner-plugin|6.0.0"
    "version-mismatch-plugin|1.0.0"
    "main-autoload-default-plugin|7.0.0"
    "main-autoload-disabled-plugin|7.1.0"
    "main-autoload-explicit-plugin|7.2.4"
    "main-autoload-env-disabled-plugin|8.9.10")

set(_plugin_dir "${CMAKE_CURRENT_BINARY_DIR}/plugins")

set(_plugin_targets)
foreach(_spec IN LISTS _plugin_specs)
    string(REPLACE "|" ";" _pair "${_spec}")
    list(GET _pair 0 _p)
    list(GET _pair 1 _v)
    string(REPLACE "-" "_" _src "${_p}")
    set(_target "eckit_test_plugin_${_src}")

    ecbuild_add_library(
        TARGET              ${_target}
        TYPE                SHARED
        NOINSTALL
        SOURCES             plugin_template.cc
        PRIVATE_DEFINITIONS ECKIT_TEST_PLUGIN_NAME="${_p}"
                            ECKIT_TEST_PLUGIN_VERSION="${_v}"
        PUBLIC_LIBS         eckit)

    # Force isolated output directory: the test plugin shared libraries must
    # NOT land in ${CMAKE_BINARY_DIR}/lib (the default ecbuild_add_library sets)
    # because that would make them visible to other tests/binaries through the
    # default loader search paths. They live only here, and are only reachable
    # via the per-test DYNAMIC_LIBRARY_PATH environment variable.
    set_target_properties(${_target} PROPERTIES
        OUTPUT_NAME                              "${_p}"
        LIBRARY_OUTPUT_DIRECTORY                 "${_plugin_dir}"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG           "${_plugin_dir}"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE         "${_plugin_dir}"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO  "${_plugin_dir}"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL      "${_plugin_dir}"
        ARCHIVE_OUTPUT_DIRECTORY                 "${_plugin_dir}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG           "${_plugin_dir}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE         "${_plugin_dir}"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO  "${_plugin_dir}"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL      "${_plugin_dir}")

    list(APPEND _plugin_targets ${_target})
endforeach()

#----------------------------------------------------------------------------------------------------------------------
# Plugin tests
#----------------------------------------------------------------------------------------------------------------------

foreach(_test
        plugin_manifest_loading
        plugin_disable
        plugin_version
        plugin_main_autoload_default
        plugin_main_autoload_disabled
        plugin_main_autoload_explicit
        plugin_main_autoload_env_disabled)
    ecbuild_add_test( TARGET       eckit_test_system_${_test}
                      SOURCES      test_system_${_test}.cc
                      LIBS         eckit
                      DEPENDS      ${_plugin_targets}
                      ENVIRONMENT  "DYNAMIC_LIBRARY_PATH=${_plugin_dir}" )
endforeach()
