ecbuild_add_executable( TARGET cpp11_futures 
	NOINSTALL
	SOURCES cpp11_futures.cc 
	CONDITION HAVE_CPP11 )