list( APPEND eckit_xpr_srcs
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
String.cc
String.h
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
Xpr.cc
Xpr.h
)


ecbuild_add_library(TARGET eckit_xpr
					INSTALL_HEADERS ALL
					HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit/xpr
					CONDITION HAVE_XPR
					SOURCES
						${eckit_xpr_srcs}
					LIBS
						eckit  )

