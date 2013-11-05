list( APPEND eckit_maths_srcs
Exp.h
Exp.cc
Const.h
Const.cc
)


ecbuild_add_library(TARGET eckit_maths
					INSTALL_HEADERS ALL
					HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit/maths
					CONDITION ECKIT_MATHS AND Boost_FOUND
					SOURCES
						${eckit_maths_srcs}
					INCLUDES
						${Boost_INCLUDE_DIRS}
					LIBS
						eckit  )

