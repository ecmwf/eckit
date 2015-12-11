ecbuild_add_library(

    TARGET eckit_multiplexer

    SOURCES
        DataSink.cc
        DataSink.h
        DummySink.cc
        DummySink.h
        MultiplexerSink.cc
        MultiplexerSink.h

    LIBS
        eckit ${GRIB_API_LIBRARIES}
)


ecbuild_add_executable(

    TARGET multiplexer
    NOINSTALL

    CONDITION HAVE_GRIB

    SOURCES
        multiplexer.cc
        DummySource.cc
        DummySource.h

    INCLUDES
        ${GRIB_API_INCLUDE_DIRS}
    LIBS
        eckit_multiplexer ${GRIB_API_LIBRARIES}
)
