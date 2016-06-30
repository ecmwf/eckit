list( APPEND eckit_maths_lib_srcs
Eigen.h
Lapack.h
Lapack.cc
Matrix.h
MatrixEigen.h
MatrixLapack.h
)

ecbuild_add_library( TARGET              eckit_maths
                     INSTALL_HEADERS     ALL
                     HEADER_DESTINATION  ${INSTALL_INCLUDE_DIR}/eckit/maths
                     SOURCES             ${eckit_maths_lib_srcs}
                     INCLUDES            "${EIGEN3_INCLUDE_DIR}"
                     LIBS                eckit "${LAPACK_LIBRARIES}" "${BLAS_LIBRARIES}" )
