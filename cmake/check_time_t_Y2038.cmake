include( CMakePushCheckState )
cmake_push_check_state()

set( CMAKE_EXTRA_INCLUDE_FILES "time.h" )
check_type_size( time_t ECKIT_SIZEOF_TIME_T )
if( ECKIT_SIZEOF_TIME_T EQUAL "8" OR ECKIT_SIZEOF_TIME_T GREATER "8" )
    ecbuild_info("time_t size ${ECKIT_SIZEOF_TIME_T}")
else()
	# once we are closer to 2038 we will upgrade this to a critical error using ecbuild_critical()
	ecbuild_warning("This system time_t has size ${ECKIT_SIZEOF_TIME_T} and therefore suffers from Y2038 problem" )
endif()

cmake_pop_check_state()

