ecbuild_add_executable(   
	TARGET      compute
	SOURCES     compute.cc 
	CONDITION   ECKIT_GRIB 
        LIBS        eckit_mining eckit_grib eckit
	)

