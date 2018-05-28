ecbuild_add_executable( TARGET sandbox_pool NOINSTALL
                        CONDITION ENABLE_SANDBOX
                        SOURCES pool.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_net NOINSTALL
                        CONDITION ENABLE_SANDBOX
                        SOURCES net.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_pipe NOINSTALL
                        CONDITION ENABLE_SANDBOX
                        SOURCES pipe.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_tcp_client NOINSTALL
                        CONDITION ENABLE_SANDBOX
                        SOURCES tcp_client.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_tcp_server NOINSTALL
                        CONDITION ENABLE_SANDBOX
                        SOURCES tcp_server.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_tcp_server_threaded NOINSTALL
                        CONDITION ENABLE_SANDBOX
                        SOURCES tcp_server_threaded.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_tcp_server_forked NOINSTALL
                        CONDITION ENABLE_SANDBOX
                        SOURCES tcp_server_forked.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET sandbox_arg NOINSTALL
                        CONDITION ENABLE_SANDBOX
                        SOURCES arg.cc
                        LIBS eckit_cmd )

ecbuild_add_executable( TARGET  sandbox_tcp_client_json NOINSTALL
                        CONDITION ENABLE_SANDBOX
                        SOURCES tcp_client_json.cc
                        LIBS eckit )

