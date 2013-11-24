ecbuild_add_executable(   
	TARGET      y-combinator
	SOURCES     y-combinator.cc 
	CONDITION   ECKIT_MATHS
	LIBS        eckit_maths eckit 
	)

ecbuild_add_executable(   
	TARGET      currying
	SOURCES     currying.cc 
	CONDITION   ECKIT_MATHS
	LIBS        eckit_maths eckit 
	)
