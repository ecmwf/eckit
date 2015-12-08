list( APPEND eckit_multiplexer_srcs
    multiplexer.cc
    DataSink.cc
    DataSink.h
    DummySink.cc
    DummySink.h
    DummySource.cc
    DummySource.h
)


ecbuild_add_executable( TARGET multiplexer
                        NOINSTALL
                        CONDITION HAVE_GRIB
                        SOURCES ${eckit_multiplexer_srcs}
                        INCLUDES ${GRIB_API_INCLUDE_DIRS}
                        LIBS eckit ${GRIB_API_LIBRARIES})
