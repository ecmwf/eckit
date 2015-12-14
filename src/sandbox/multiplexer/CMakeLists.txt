list( APPEND eckit_multiplexer_srcs
    DataSink.cc
    DataSink.h
    DummySink.cc
    DummySink.h
    DummySource.cc
    DummySource.h
    MultiplexerSink.cc
    MultiplexerSink.h
)

ecbuild_add_library( TARGET eckit_sandbox_multiplexer
                     NOINSTALL
                     CONDITION HAVE_GRIB
                     SOURCES ${eckit_multiplexer_srcs}
                     LIBS eckit ${GRIB_API_LIBRARIES})


ecbuild_add_executable( TARGET multiplexer
                        NOINSTALL
                        CONDITION HAVE_GRIB
                        SOURCES ${eckit_multiplexer_srcs} multiplexer.cc
                        INCLUDES ${GRIB_API_INCLUDE_DIRS}
                        LIBS eckit ${GRIB_API_LIBRARIES})

