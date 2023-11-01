ecbuild_add_executable( TARGET      eckit_version
                        OUTPUT_NAME eckit-version
                        CONDITION   HAVE_BUILD_TOOLS
                        SOURCES     eckit-version.cc
                        LIBS        eckit_option eckit )

ecbuild_add_executable( TARGET      eckit_info
                        OUTPUT_NAME eckit-info
                        CONDITION   HAVE_BUILD_TOOLS
                        SOURCES     eckit-info.cc
                        LIBS        eckit_option eckit )

### NOT TO INSTALL

ecbuild_add_executable( TARGET      dhcopy
                        OUTPUT_NAME eckit-dhcopy
                        CONDITION   HAVE_BUILD_TOOLS
                        NOINSTALL
                        SOURCES     dhcopy.cc
                        LIBS        eckit_option eckit )

ecbuild_add_executable( TARGET      syslog_server
                        OUTPUT_NAME eckit-syslog-server
                        CONDITION   HAVE_BUILD_TOOLS
                        NOINSTALL
                        SOURCES     syslog-server.cc
                        LIBS        eckit )

ecbuild_add_executable( TARGET      syslog_client
                        OUTPUT_NAME eckit-syslog-client
                        CONDITION   HAVE_BUILD_TOOLS
                        NOINSTALL
                        SOURCES     syslog-client.cc
                        LIBS        eckit )

ecbuild_add_executable( TARGET      eckit_rsync
                        OUTPUT_NAME eckit-rsync
                        CONDITION   HAVE_BUILD_TOOLS AND eckit_HAVE_RSYNC
                        NOINSTALL
                        SOURCES     rsync.cc
                        LIBS        eckit_option eckit )

ecbuild_add_executable( TARGET      eckit_hash
                        OUTPUT_NAME eckit-hash
                        CONDITION   HAVE_BUILD_TOOLS
                        NOINSTALL
                        SOURCES     eckit-hash.cc
                        LIBS        eckit_option eckit )
