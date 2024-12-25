
if( eckit_HAVE_S3 )

    set( S3_TEST_HOST       "minio" )
    set( S3_TEST_PORT       "9000" )
    set( S3_TEST_ENDPOINT   ${S3_TEST_HOST}:${S3_TEST_PORT} )
    set( S3_TEST_REGION     "eu-central-1" )
    set( S3_TEST_BUCKET     "eckit-test-s3-bucket" )
    set( S3_TEST_OBJECT     "eckit-test-s3-object" )

    configure_file( test_s3_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/test_s3_config.h @ONLY )

    configure_file( S3Config.yaml.in        S3Config.yaml       @ONLY )
    configure_file( S3Credentials.yaml.in   S3Credentials.yaml  @ONLY )

    set( test_srcs client handle )

    foreach( _test ${test_srcs} )

        ecbuild_add_test(
            TARGET      eckit_test_s3_${_test}
            SOURCES     test_s3_${_test}.cc
            INCLUDES    test_s3_config.h ${AWSSDK_INCLUDE_DIRS}
            LIBS        eckit ${AWSSDK_LINK_LIBRARIES}
            ENVIRONMENT "${_test_environment}"
        )

    endforeach()

endif( eckit_HAVE_S3 )
