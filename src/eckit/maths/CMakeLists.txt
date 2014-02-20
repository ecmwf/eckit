list( APPEND eckit_maths_srcs
BinaryOperator.cc
BinaryOperator.h
BinaryPredicate.cc
BinaryPredicate.h
Boolean.cc
Boolean.h
UnaryOperator.cc
UnaryOperator.h
Call.cc
Call.h
Math.cc
Math.h
Bind.cc
Bind.h
Expression.cc
Expression.h
Function.cc
Function.h
Count.cc
Count.h
Map.cc
Map.h
Merge.cc
Merge.h
IfElse.cc
IfElse.h
Lambda.cc
Lambda.h
Filter.cc
Filter.h
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
ZipWith.cc
ZipWith.h
Scope.cc
Scope.h
Param.cc
Param.h
ParamDef.cc
ParamDef.h
Undef.cc
Undef.h
Take.h
Take.cc
)


ecbuild_add_library(TARGET eckit_maths
					INSTALL_HEADERS ALL
					HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit/maths
					CONDITION HAVE_MATHS AND Boost_FOUND
					SOURCES
						${eckit_maths_srcs}
					INCLUDES
						${Boost_INCLUDE_DIRS}
					LIBS
						eckit  )

