ecbuild_add_test(
	TARGET      y-combinator
	SOURCES     y-combinator.cc 
	CONDITION HAVE_MATHS AND Boost_FOUND
	INCLUDES    ${Boost_INCLUDE_DIRS}
	LIBS        eckit_maths eckit
	)

ecbuild_add_test(
	TARGET      currying
	SOURCES     currying.cc 
	CONDITION HAVE_MATHS AND Boost_FOUND
	INCLUDES    ${Boost_INCLUDE_DIRS}
	LIBS        eckit_maths eckit
	)

ecbuild_add_test(
	TARGET      optimise
	SOURCES     optimise.cc 
	CONDITION HAVE_MATHS AND Boost_FOUND
	INCLUDES    ${Boost_INCLUDE_DIRS}
	LIBS        eckit_maths eckit
	)

ecbuild_add_test(
	TARGET      quicksort
	SOURCES     quicksort.cc 
	CONDITION HAVE_MATHS AND Boost_FOUND
	INCLUDES    ${Boost_INCLUDE_DIRS}
	LIBS        eckit_maths eckit
	)
