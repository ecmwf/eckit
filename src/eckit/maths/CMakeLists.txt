list( APPEND eckit_maths_lib_srcs
    Eigen.h
    Lapack.cc
    Lapack.h
    Matrix.h
    MatrixEigen.h
    MatrixLapack.h
)

if( eckit_HAVE_ECKIT_GEO )
    # for eckit::geo
    list( APPEND eckit_maths_lib_srcs
        Matrix3.h
    )
endif()

ecbuild_add_library( TARGET              eckit_maths
                     TYPE                SHARED
                     INSTALL_HEADERS     ALL
                     HEADER_DESTINATION  ${INSTALL_INCLUDE_DIR}/eckit/maths
                     SOURCES             ${eckit_maths_lib_srcs}
                     PUBLIC_LIBS         eckit
                     PRIVATE_LIBS        "${LAPACK_LIBRARIES}" "${BLAS_LIBRARIES}" )

if( HAVE_EIGEN )
    # Add include directories with "SYSTEM" to avoid warnings from within Eigen headers
    target_include_directories(eckit_maths SYSTEM PUBLIC ${EIGEN3_INCLUDE_DIRS})
endif()
