list( APPEND eckit_maths_srcs
BinaryFunc.cc
BinaryFunc.h
UnaryFunc.cc
UnaryFunc.h
Exp.cc
Exp.h
Func.cc
Func.h
Count.cc
Count.h
FMap.cc
FMap.h
Reduce.cc
Reduce.h
Linear.cc
Linear.h
List.h
List.cc
ProdAdd.cc
ProdAdd.h
Optimiser.cc
Optimiser.h
Scalar.cc
Scalar.h
Value.cc
Value.h
Vector.cc
Vector.h
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

