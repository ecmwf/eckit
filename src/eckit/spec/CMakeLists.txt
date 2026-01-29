
ecbuild_add_library(
    TARGET eckit_spec
    TYPE SHARED
    INSTALL_HEADERS ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/spec
    PUBLIC_LIBS eckit

    PUBLIC_INCLUDES
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>

    SOURCES
        Custom.cc
        Custom.h
        Exceptions.cc
        Exceptions.h
        Generator.h
        Layered.cc
        Layered.h
        LibEcKitSpec.cc
        LibEcKitSpec.h
        Spec.cc
        Spec.h
)
