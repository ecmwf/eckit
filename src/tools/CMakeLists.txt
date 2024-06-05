ecbuild_add_executable( TARGET      eckit_version
                        OUTPUT_NAME eckit-version
                        SOURCES     eckit-version.cc
                        LIBS        eckit_option eckit )

if( NOT eckit_HAVE_BUILD_TOOLS )
    return()
endif()

ecbuild_add_executable( TARGET      eckit_info
                        OUTPUT_NAME eckit-info
                        SOURCES     eckit-info.cc
                        LIBS        eckit_option eckit )

ecbuild_add_executable( TARGET      eckit_codec_list
                        OUTPUT_NAME eckit-codec-list
                        CONDITION   eckit_HAVE_ECKIT_CODEC
                        SOURCES     eckit-codec-list.cc
                        LIBS        eckit_codec eckit_option )

### NOT TO INSTALL

ecbuild_add_executable( TARGET      dhcopy
                        OUTPUT_NAME eckit-dhcopy
                        NOINSTALL
                        SOURCES     dhcopy.cc
                        LIBS        eckit_option eckit )

ecbuild_add_executable( TARGET      syslog_server
                        OUTPUT_NAME eckit-syslog-server
                        NOINSTALL
                        SOURCES     syslog-server.cc
                        LIBS        eckit )

ecbuild_add_executable( TARGET      syslog_client
                        OUTPUT_NAME eckit-syslog-client
                        NOINSTALL
                        SOURCES     syslog-client.cc
                        LIBS        eckit )

ecbuild_add_executable( TARGET      eckit_rsync
                        OUTPUT_NAME eckit-rsync
                        CONDITION   eckit_HAVE_RSYNC
                        NOINSTALL
                        SOURCES     rsync.cc
                        LIBS        eckit_option eckit )

ecbuild_add_executable( TARGET      eckit_hash
                        OUTPUT_NAME eckit-hash
                        NOINSTALL
                        SOURCES     eckit-hash.cc
                        LIBS        eckit_option eckit )
