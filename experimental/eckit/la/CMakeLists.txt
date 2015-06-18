list( APPEND eckit_la_srcs
types.h
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
Matrix.h
SparseMatrix.h
Triplet.h
Vector.h
)

ecbuild_add_library( TARGET             eckit_la
                     INSTALL_HEADERS    ALL
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/la
                     SOURCES            ${eckit_la_srcs}
                     INCLUDES           ${MKL_INCLUDE_DIRS} ${CUDA_INCLUDE_DIRS}
                     LIBS               eckit ${MKL_LIBRARIES} ${CUDA_LIBRARIES} )

if (CUDA_FOUND)
cuda_add_cublas_to_target( eckit_la )
endif()
