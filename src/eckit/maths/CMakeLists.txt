list(APPEND eckit_maths_public_libs eckit)
list(APPEND eckit_maths_sources
    Eigen.h
    Lapack.h
    Lapack.cc
    Matrix.h
    MatrixEigen.h
    MatrixLapack.h)

if(eckit_HAVE_CONVEX_HULL)
    list(APPEND eckit_maths_sources ConvexHull.h ConvexHullN.h Qhull.cc Qhull.h)
    list(APPEND eckit_maths_public_libs Qhull::Qhull)
endif()

ecbuild_add_library(
    TARGET              eckit_maths
    TYPE                SHARED
    INSTALL_HEADERS     ALL
    HEADER_DESTINATION  ${INSTALL_INCLUDE_DIR}/eckit/maths
    SOURCES             ${eckit_maths_sources}
    PRIVATE_LIBS        "${LAPACK_LIBRARIES}" "${BLAS_LIBRARIES}"
    PUBLIC_INCLUDES     "${EIGEN3_INCLUDE_DIRS}"
    PUBLIC_LIBS         ${eckit_maths_public_libs})

