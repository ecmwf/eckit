list( APPEND eckit_la_srcs
types.h
LinearAlgebraArmadillo.cc
LinearAlgebraArmadillo.h
LinearAlgebraBase.h
LinearAlgebraCUDA.cc
LinearAlgebraCUDA.h
LinearAlgebraEigen.cc
LinearAlgebraEigen.h
LinearAlgebraFactory.cc
LinearAlgebraFactory.h
LinearAlgebraGeneric.cc
LinearAlgebraGeneric.h
LinearAlgebraMKL.cc
LinearAlgebraMKL.h
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
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/experimental/eckit/la
                     SOURCES            ${eckit_la_srcs}
                     INCLUDES           ${ARMADILLO_INCLUDE_DIRS} ${CUDA_INCLUDE_DIRS} ${MKL_INCLUDE_DIRS}
                     LIBS               eckit ${ARMADILLO_LIBRARIES} ${MKL_LIBRARIES} ${CUDA_LIBRARIES} ${CUDA_cusparse_LIBRARY} )

if (CUDA_FOUND)
cuda_add_cublas_to_target( eckit_la )
endif()
