ecbuild_add_executable( TARGET      multiplexer
                        NOINSTALL
                        SOURCES     multiplexer.cc DataSink.cc DataSink.h DummySink.cc DummySink.h
                        LIBS        eckit )
