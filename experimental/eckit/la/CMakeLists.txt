list( APPEND eckit_la_srcs
types.h
LinearAlgebraBase.h
LinearAlgebraGeneric.cc
LinearAlgebraGeneric.h
Matrix.h
Vector.h
)

ecbuild_add_library( TARGET             eckit_la
                     INSTALL_HEADERS    ALL
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/la
                     SOURCES            ${eckit_la_srcs}
                     LIBS               eckit )
