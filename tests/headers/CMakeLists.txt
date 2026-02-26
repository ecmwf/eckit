# Self-contained header compilation test
#
# Every public header in eckit must compile on its own, without requiring
# the consumer to include other headers first or in a particular order.
# This is essential for a library: downstream projects include eckit
# headers in arbitrary combinations and orders, and a header that silently
# depends on a transitive include will break when unrelated changes
# elsewhere shuffle include graphs. Self-contained headers eliminate this
# class of fragile, hard-to-diagnose build failures for all users.
#
# This test generates a .cc file per header (containing only that single
# #include) and compiles them into one target. A compilation failure
# immediately identifies the offending header.
#
# Gate: -DENABLE_TEST_SELF_CONTAINED_HEADERS=ON  (off by default to avoid
# the extra compile cost on every regular build).

# Glob all public headers under src/eckit/
file( GLOB_RECURSE _all_headers
      RELATIVE ${PROJECT_SOURCE_DIR}/src
      ${PROJECT_SOURCE_DIR}/src/eckit/*.h )

# Exclusions
list( FILTER _all_headers EXCLUDE REGEX "^eckit/contrib/" )

set( _skip_headers
    # Must be included via Matrix.h (backend selector guard)
    eckit/maths/MatrixEigen.h
    eckit/maths/MatrixLapack.h
    # Require optional external libraries not always available
    eckit/linalg/detail/CUDA.h          # CUDA runtime
    eckit/linalg/detail/HIP.h           # HIP runtime
    eckit/io/rados/RadosCluster.h       # librados (Ceph)
    eckit/io/rados/RadosHandle.h        # librados (Ceph)
    eckit/utils/MD4.h                   # OpenSSL
    eckit/utils/SHA1.h                  # OpenSSL
    eckit/geo/area/library/Shapefile.h  # shapelib
    # Require MPI headers (only available when ENABLE_MPI=ON)
    eckit/mpi/Parallel.h
    eckit/mpi/ParallelGroup.h
    eckit/mpi/ParallelRequest.h
    eckit/mpi/ParallelStatus.h
    # Coupled to ODB — reference odb:: types from a separate project
    eckit/sql/expression/function/FunctionMATCH.h
    eckit/sql/expression/ShiftedColumnExpression.h
    eckit/sql/SQLIteratorOutput.h
    eckit/sql/SQLMATCHSubquerySession.h
    eckit/sql/SQLMATCHSubquerySessionOutput.h
)
list( REMOVE_ITEM _all_headers ${_skip_headers} )

# Generate one .cc per header
set( _generated_sources )
foreach( _header ${_all_headers} )
    string( REPLACE "/" "_" _stem ${_header} )
    string( REPLACE "." "_" _stem ${_stem} )
    set( _src "${CMAKE_CURRENT_BINARY_DIR}/${_stem}.cc" )
    file( GENERATE OUTPUT ${_src}
          CONTENT "#include \"${_header}\"\n" )
    list( APPEND _generated_sources ${_src} )
endforeach()

# Compile-only check: an OBJECT library verifies each header compiles
# standalone without needing a main() or link step.
if( HAVE_TEST_SELF_CONTAINED_HEADERS )
    add_library( eckit_test_headers_selfcontained OBJECT ${_generated_sources} )
    target_link_libraries( eckit_test_headers_selfcontained PUBLIC eckit )
endif()
