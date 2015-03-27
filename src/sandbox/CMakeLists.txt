ecbuild_get_cxx11_flags( CXX11_FLAGS )

ecbuild_add_executable( TARGET      cpp11_futures
                        NOINSTALL
                        SOURCES     cpp11_futures.cc
                        CXXFLAGS    ${CXX11_FLAGS}
                        CONDITION   HAVE_CPP11 )
