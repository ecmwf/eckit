unset( eckit_la_pincludes )
unset( eckit_la_plibs )
list( APPEND eckit_la_srcs
      BackendRegistry.h
      LinearAlgebra.cc
      LinearAlgebra.h
      LinearAlgebraDense.cc
      LinearAlgebraDense.h
      LinearAlgebraSparse.cc
      LinearAlgebraSparse.h
      Matrix.cc
      Matrix.h
      SparseMatrix.cc
      SparseMatrix.h
      Tensor.cc
      Tensor.h
      Triplet.cc
      Triplet.h
      Vector.cc
      Vector.h
      allocator/BufferAllocator.cc
      allocator/BufferAllocator.h
      allocator/NonOwningAllocator.cc
      allocator/NonOwningAllocator.h
      allocator/StandardAllocator.cc
      allocator/StandardAllocator.h
      allocator/StandardContainerAllocator.cc
      allocator/StandardContainerAllocator.h
      dense/LinearAlgebraGeneric.cc
      dense/LinearAlgebraGeneric.h
      sparse/LinearAlgebraGeneric.cc
      sparse/LinearAlgebraGeneric.h
      types.h )

if( eckit_HAVE_ARMADILLO )
    list( APPEND eckit_la_srcs
          dense/LinearAlgebraArmadillo.cc
          dense/LinearAlgebraArmadillo.h )
    list( APPEND eckit_la_pincludes "${ARMADILLO_INCLUDE_DIRS}" )
    list( APPEND eckit_la_plibs "${ARMADILLO_LIBRARIES}" )
endif()

if( eckit_HAVE_CUDA )
    list( APPEND eckit_la_srcs
          dense/LinearAlgebraCUDA.cc
          dense/LinearAlgebraCUDA.h
          detail/CUDA.h
          sparse/LinearAlgebraCUDA.cc
          sparse/LinearAlgebraCUDA.h )
    list( APPEND eckit_la_plibs CUDA::cudart CUDA::cusparse CUDA::cublas )
endif()

if( eckit_HAVE_HIP )
    list( APPEND eckit_la_srcs
          sparse/LinearAlgebraHIP.cc
          sparse/LinearAlgebraHIP.h )
    list( APPEND eckit_la_plibs hip::host roc::hipsparse )
endif()


if( eckit_HAVE_EIGEN )
    list( APPEND eckit_la_srcs
          dense/LinearAlgebraEigen.cc
          dense/LinearAlgebraEigen.h
          sparse/LinearAlgebraEigen.cc
          sparse/LinearAlgebraEigen.h )
endif()

if( eckit_HAVE_LAPACK )
    list( APPEND eckit_la_srcs
          dense/LinearAlgebraLAPACK.cc
          dense/LinearAlgebraLAPACK.h )
    list( APPEND eckit_la_plibs "${LAPACK_LIBRARIES}" )
endif()

if( eckit_HAVE_MKL )
    list( APPEND eckit_la_srcs
          dense/LinearAlgebraMKL.cc
          dense/LinearAlgebraMKL.h
          sparse/LinearAlgebraMKL.cc
          sparse/LinearAlgebraMKL.h )
    list( APPEND eckit_la_pincludes "${MKL_INCLUDE_DIRS}" )
    list( APPEND eckit_la_plibs "${MKL_LIBRARIES}" )
endif()

if( eckit_HAVE_VIENNACL )
    list( APPEND eckit_la_srcs
          dense/LinearAlgebraViennaCL.cc
          dense/LinearAlgebraViennaCL.h
          sparse/LinearAlgebraViennaCL.cc
          sparse/LinearAlgebraViennaCL.h )
    list( APPEND eckit_la_pincludes "${VIENNACL_INCLUDE_DIRS}" )
endif()

if( eckit_HAVE_OMP )
    list( APPEND eckit_la_plibs OpenMP::OpenMP_CXX )
endif()

ecbuild_add_library( TARGET             eckit_linalg TYPE SHARED
                     INSTALL_HEADERS    ALL
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/linalg
                     SOURCES            ${eckit_la_srcs}
                     PRIVATE_INCLUDES   ${eckit_la_pincludes}
                     PUBLIC_LIBS        eckit
                     PRIVATE_LIBS       ${eckit_la_plibs} )

if( eckit_HAVE_EIGEN )
    if(NOT EIGEN3_INCLUDE_DIRS AND TARGET Eigen3::Eigen)
        get_target_property(EIGEN3_INCLUDE_DIRS Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
    endif()

    # Add include directories with "SYSTEM" to avoid warnings from within Eigen headers
    target_include_directories( eckit_linalg SYSTEM PRIVATE ${EIGEN3_INCLUDE_DIRS} )
endif()
