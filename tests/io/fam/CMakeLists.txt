if( HAVE_OPENFAM )
    set( _fam_extra_libs LibOPENFAM )
    # only relevant for testing with a real OpenFAM servers running in docker environment.
    list( APPEND _fam_environment ${_test_environment}
        "OPENFAM_ROOT=/workspace/cis-rpc_meta-direct_mem-rpc"
        "ECKIT_FAM_TEST_ENDPOINT=172.26.0.2:8880"
    )
else()
    set( _fam_extra_libs "" )
    list( APPEND _fam_environment
        "ECKIT_FAM_MOCK_SHM_SIZE=268435456"
    )
endif()

foreach( _test fam_handle fam_list fam_map fam_name fam_object fam_path fam_property fam_region fam_session_manager fam_uri_manager )
  ecbuild_add_test( TARGET      eckit_test_${_test}
                    SOURCES     test_${_test}.cc
                    CONDITION   HAVE_OPENFAM
                    LABELS      fam
                    ENVIRONMENT "${_fam_environment}"
                    LIBS        eckit ${_fam_extra_libs} )
endforeach()
