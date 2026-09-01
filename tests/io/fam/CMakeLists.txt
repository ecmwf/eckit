# Only the mock shares a size-limited /dev/shm segment; real OpenFAM has no such constraint.
set( _fam_test_properties "" )

if( eckit_HAVE_OPENFAM_MOCK )
    # Mock takes priority: per-process shm isolation via PID-mangled endpoints.
    list( APPEND _fam_environment
        "ECKIT_FAM_MOCK_SHM_SIZE=33554432"
    )
    # Serialise FAM tests: each mock segment reserves ~32 MiB of the shared, size-limited /dev/shm,
    # so running many at once (ctest -j) exhausts tmpfs.
    set( _fam_test_properties TEST_PROPERTIES RESOURCE_LOCK famfdb_mock_shm )
elseif( eckit_HAVE_OPENFAM )
    # Real OpenFAM servers running in docker environment.
    list( APPEND _fam_environment ${_test_environment}
        "OPENFAM_ROOT=/workspace/cis-rpc_meta-direct_mem-rpc"
        "ECKIT_FAM_TEST_ENDPOINT=172.26.0.2:8880"
    )
endif()

foreach( _test fam_handle fam_list fam_map fam_map_64 fam_map_concurrent fam_name fam_object fam_path fam_property fam_region fam_session_manager fam_uri_manager )
  ecbuild_add_test( TARGET      eckit_test_${_test}
                    SOURCES     test_${_test}.cc
                    CONDITION   eckit_HAVE_OPENFAM
                    LABELS      fam
                    ENVIRONMENT "${_fam_environment}"
                    ${_fam_test_properties}
                    LIBS        eckit )
endforeach()
