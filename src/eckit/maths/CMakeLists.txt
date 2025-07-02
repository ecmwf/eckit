list(APPEND eckit_maths_private_libs "${LAPACK_LIBRARIES}" "${BLAS_LIBRARIES}")
list(APPEND eckit_maths_sources
    Eigen.h
    FloatingPointExceptions.cc
    FloatingPointExceptions.h
    Lapack.cc
    Lapack.h
    Matrix.h
    Matrix3.h
    MatrixEigen.h
    MatrixLapack.h
)

if(CMAKE_CXX_COMPILER_ID MATCHES Intel)
    set_property(
      SOURCE    FloatingPointExceptions.cc
      APPEND
      PROPERTY  COMPILE_OPTIONS -fp-model precise)
    # See https://github.com/ecmwf/eckit/pull/206 
    # Without this, we get following error in a Intel Release build (-O3):
    #    eckit/maths/FloatingPointExceptions.cc:26:14: error: '#pragma STDC FENV_ACCESS ON' is illegal when precise is disabled
    #     | #pragma STDC FENV_ACCESS ON
    #     |              ^
endif()

if(eckit_HAVE_CONVEX_HULL)
    list(APPEND eckit_maths_sources ConvexHull.h ConvexHullN.h Qhull.cc Qhull.h)
    list(APPEND eckit_maths_private_libs Qhull::Qhull)
endif()

ecbuild_add_library(
    TARGET             eckit_maths
    TYPE               SHARED
    INSTALL_HEADERS    ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/maths
    SOURCES            ${eckit_maths_sources}
    PRIVATE_LIBS       ${eckit_maths_private_libs}
    PUBLIC_LIBS        eckit
)

if(eckit_HAVE_EIGEN)
    if(NOT EIGEN3_INCLUDE_DIRS AND TARGET Eigen3::Eigen)
        get_target_property(EIGEN3_INCLUDE_DIRS Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
    endif()

    # Add include directories with "SYSTEM" to avoid warnings from within Eigen headers
    target_include_directories(eckit_maths SYSTEM PUBLIC ${EIGEN3_INCLUDE_DIRS})
endif()
