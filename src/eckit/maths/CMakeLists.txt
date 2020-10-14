list( APPEND eckit_maths_lib_srcs
Eigen.h
Lapack.h
Lapack.cc
Matrix.h
MatrixEigen.h
MatrixLapack.h
)

ecbuild_add_library( TARGET              eckit_maths TYPE SHARED
                     INSTALL_HEADERS     ALL
                     HEADER_DESTINATION  ${INSTALL_INCLUDE_DIR}/eckit/maths
                     SOURCES             ${eckit_maths_lib_srcs}
                     PUBLIC_INCLUDES     "${EIGEN3_INCLUDE_DIRS}"
                     PUBLIC_LIBS         eckit
                     PRIVATE_LIBS        "${LAPACK_LIBRARIES}" "${BLAS_LIBRARIES}" )
