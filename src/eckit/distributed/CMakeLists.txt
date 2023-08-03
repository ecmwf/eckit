list( APPEND eckit_distributed_srcs
Actor.cc
Actor.h
Consumer.cc
Consumer.h
Message.cc
Message.h
NoTransport.cc
NoTransport.h
Producer.cc
Producer.h
Transport.cc
Transport.h
TransportHandle.cc
TransportHandle.h
TransportStatistics.cc
TransportStatistics.h
tcp/TCPTransport.cc
tcp/TCPTransport.h
)

if( HAVE_MPI )

    list( APPEND eckit_distributed_srcs
        mpi/MPITransport.cc
        mpi/MPITransport.h
        )
endif()

ecbuild_add_library( TARGET eckit_distributed TYPE SHARED
                     INSTALL_HEADERS ALL
                     HEADER_DESTINATION
                         ${INSTALL_INCLUDE_DIR}/eckit/distributed
                     SOURCES
                         ${eckit_distributed_srcs}
                     PUBLIC_LIBS
                         eckit )

if ( HAVE_MPI )
    target_link_libraries( eckit_distributed PUBLIC eckit_mpi )
endif()
