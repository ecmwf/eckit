list( APPEND eckit_xpr_srcs
BinaryOperator.cc
BinaryOperator.h
BinaryPredicate.cc
BinaryPredicate.h
Bind.cc
Bind.h
Boolean.cc
Boolean.h
Call.cc
Call.h
Count.cc
Count.h
DoWhile.cc
DoWhile.h
Expression.cc
Expression.h
Filter.cc
Filter.h
ForEach.cc
ForEach.h
Function.cc
Function.h
IfElse.cc
IfElse.h
Integer.cc
Integer.h
Lambda.cc
Lambda.h
Linear.cc
Linear.h
List.cc
List.h
Map.cc
Map.h
Merge.cc
Merge.h
Optimiser.cc
Optimiser.h
Param.cc
Param.h
ParamDef.cc
ParamDef.h
ProdAdd.cc
ProdAdd.h
Real.cc
Real.h
Reduce.cc
Reduce.h
Scope.cc
Scope.h
String.cc
String.h
Take.cc
Take.h
UnaryOperator.cc
UnaryOperator.h
Undef.cc
Undef.h
Value.cc
Value.h
Vector.cc
Vector.h
Xpr.cc
Xpr.h
ZipWith.cc
ZipWith.h
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

