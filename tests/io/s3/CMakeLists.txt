
if( eckit_HAVE_S3 )

    file(
        COPY S3Config.yaml S3Credentials.yaml
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )

    ecbuild_add_test(
        TARGET      eckit_test_s3client
        SOURCES     test_s3client.cc
        INCLUDES    ${AWSSDK_INCLUDE_DIRS}
        LIBS        eckit ${AWSSDK_LINK_LIBRARIES}
    )

    ecbuild_add_test(
        TARGET      eckit_test_s3handle
        SOURCES     test_s3handle.cc
        INCLUDES    ${AWSSDK_INCLUDE_DIRS}
        LIBS        eckit ${AWSSDK_LINK_LIBRARIES}
    )

endif( eckit_HAVE_S3 )
