list( APPEND eckit_maths_lib_srcs
    Eigen.h
    Lapack.cc
    Lapack.h
    Matrix.h
    MatrixEigen.h
    MatrixLapack.h )

list( APPEND eckit_maths_incs "${EIGEN3_INCLUDE_DIRS}" )
list( APPEND eckit_maths_libs "${LAPACK_LIBRARIES}" "${BLAS_LIBRARIES}" )

if( eckit_HAVE_CONVEX_HULL )
    list( APPEND eckit_geo_srcs
        ConvexHull.h
        ConvexHullN.cc
        ConvexHullN.h
        Qhull.cc
        Qhull.h
        Triangulation.h
        Triangulation3.cc
        Triangulation3.h )
    list( APPEND eckit_maths_libs Qhull::Qhull )
endif()

ecbuild_add_library( TARGET              eckit_maths TYPE SHARED
                     INSTALL_HEADERS     ALL
                     HEADER_DESTINATION  ${INSTALL_INCLUDE_DIR}/eckit/maths
                     SOURCES             ${eckit_maths_lib_srcs}
                     PUBLIC_INCLUDES     ${eckit_maths_lib_incs}
                     PUBLIC_LIBS         eckit
                     PRIVATE_LIBS        ${eckit_maths_libs} )
