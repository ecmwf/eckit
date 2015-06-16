list( APPEND eckit_la_srcs
types.h
LinearAlgebraBase.h
LinearAlgebraEigen.cc
LinearAlgebraEigen.h
LinearAlgebraGeneric.cc
LinearAlgebraGeneric.h
LinearAlgebraMKL.cc
LinearAlgebraMKL.h
Matrix.h
Vector.h
)

ecbuild_add_library( TARGET             eckit_la
                     INSTALL_HEADERS    ALL
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/la
                     SOURCES            ${eckit_la_srcs}
                     INCLUDES           ${MKL_INCLUDE_DIRS}
                     LIBS               eckit ${MKL_LIBRARIES} )
