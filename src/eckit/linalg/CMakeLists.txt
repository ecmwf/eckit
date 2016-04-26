list( APPEND eckit_la_srcs
types.h
LinearAlgebra.cc
LinearAlgebra.h
LinearAlgebraArmadillo.cc
LinearAlgebraArmadillo.h
LinearAlgebraCUDA.cc
LinearAlgebraCUDA.h
LinearAlgebraEigen.cc
LinearAlgebraEigen.h
LinearAlgebraGeneric.cc
LinearAlgebraGeneric.h
LinearAlgebraMKL.cc
LinearAlgebraMKL.h
LinearAlgebraViennaCL.cc
LinearAlgebraViennaCL.h
Matrix.cc
Matrix.h
SparseMatrix.cc
SparseMatrix.h
Triplet.h
Vector.cc
Vector.h
)

ecbuild_add_library( TARGET             eckit_la
                     INSTALL_HEADERS    ALL
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/linalg
                     SOURCES            ${eckit_la_srcs}
                     INCLUDES           "${ARMADILLO_INCLUDE_DIRS}"
                                        "${CUDA_INCLUDE_DIRS}"
                                        "${MKL_INCLUDE_DIRS}"
                                        "${VIENNACL_INCLUDE_DIRS}"
                     LIBS               eckit
                                        "${ARMADILLO_LIBRARIES}"
                                        "${CUDA_LIBRARIES}" "${CUDA_cusparse_LIBRARY}"
                                        "${MKL_LIBRARIES}" )

if (CUDA_FOUND)
cuda_add_cublas_to_target( eckit_la )
endif()
