ecbuild_add_executable( TARGET sandbox_pool NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES pool.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_net NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES net.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_pipe NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES pipe.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_tcp_client NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES tcp_client.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_tcp_server NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES tcp_server.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_tcp_server_threaded NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES tcp_server_threaded.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_tcp_server_forked NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES tcp_server_forked.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET sandbox_arg NOINSTALL
                        CONDITION HAVE_SANDBOX AND HAVE_ECKIT_CMD
                        SOURCES arg.cc
                        LIBS eckit_cmd )

ecbuild_add_executable( TARGET  sandbox_tcp_client_json NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES tcp_client_json.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_mpmc NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES mpmc.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_shint NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES shint.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_memsharedlimit NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES memsharedlimit.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET sandbox_mpiobjsend NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES mpiobjsend.cc
                        LIBS eckit_mpi eckit )

ecbuild_add_executable( TARGET sandbox_udp_server NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES udp_server.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET sandbox_udp_client NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES udp_client.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_multi_socket_server NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES multi_socket_server.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_multi_socket_client NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES multi_socket_client.cc
                        LIBS eckit )

ecbuild_add_executable( TARGET  sandbox_shmem_ipc NOINSTALL
                        CONDITION HAVE_SANDBOX
                        SOURCES shmem_ipc.cc
                        LIBS eckit )
