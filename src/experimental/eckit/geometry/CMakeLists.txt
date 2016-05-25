list( APPEND eckit_geometry_srcs
KPoint.h
Point2.cc
Point2.h
Point3.cc
Point3.h
)

ecbuild_add_library(
    TARGET             eckit_geometry
    INSTALL_HEADERS    ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/geometry
    SOURCES            ${eckit_geometry_srcs}
    LIBS               eckit )

