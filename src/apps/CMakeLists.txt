ecbuild_add_executable( TARGET      eckit_version
                        OUTPUT_NAME eckit-version
                        SOURCES     eckit-version.cc
                        LIBS        eckit )

ecbuild_add_executable( TARGET      syslog_server
                        OUTPUT_NAME syslog-server
                        SOURCES     syslog-server.cc
                        LIBS        eckit )

ecbuild_add_executable( TARGET      syslog_client
                        OUTPUT_NAME syslog-client
                        SOURCES     syslog-client.cc
                        LIBS        eckit )

