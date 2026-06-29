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

ecbuild_add_executable( TARGET      eckit_fraction
                        OUTPUT_NAME eckit-fraction
                        SOURCES     eckit-fraction.cc
                        LIBS        eckit_option eckit )

ecbuild_add_executable( TARGET      eckit_monitor_dump
                        OUTPUT_NAME eckit-monitor-dump
                        SOURCES     eckit-monitor-dump.cc
                        LIBS        eckit_option eckit )

ecbuild_add_executable( TARGET      eckit_codec_list
                        OUTPUT_NAME eckit-codec-list
                        CONDITION   eckit_HAVE_ECKIT_CODEC
                        SOURCES     eckit-codec-list.cc
                        LIBS        eckit_codec eckit_option )

foreach( _name
        eckit-geo-area-list
        eckit-geo-gaussian-latitudes
        eckit-geo-grid
        eckit-geo-grid-cache
        eckit-geo-grid-list
        eckit-geo-grid-nearest
        eckit-geo-grid-spec )
    string(REPLACE "-" "_" _target ${_name})
    ecbuild_add_executable( TARGET      ${_target}
                            OUTPUT_NAME ${_name}
                            SOURCES     ${_name}.cc
                            CONDITION   eckit_HAVE_ECKIT_GEO
                            LIBS        eckit_geo eckit_option )
endforeach()


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

ecbuild_add_executable( TARGET      eckit_hash
                        OUTPUT_NAME eckit-hash
                        NOINSTALL
                        SOURCES     eckit-hash.cc
                        LIBS        eckit_option eckit )
