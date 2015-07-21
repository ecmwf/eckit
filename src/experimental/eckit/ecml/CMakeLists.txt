list( APPEND eckit_ecml_srcs
parser/Request.h
parser/Request.cc
parser/Cell.h
parser/Cell.cc
parser/List.h
parser/List.cc
parser/RequestParser.h
parser/RequestParser.cc
# From utils
DataHandleFactory.h
DataHandleFactory.cc
# From config
DynamicParametrisation.h
DynamicParametrisation.cc
Environment.h
Environment.cc
ExecutionContext.h
ExecutionContext.cc
# From utils
FileHandleFactory.h
FileHandleFactory.cc
Interpreter.h
Interpreter.cc
Module.cc
Module.h
ParameterizedRequestHandler.h
ParameterizedRequestHandler.cc
RequestHandler.h
RequestHandler.cc
SpecialFormHandler.h
SpecialFormHandler.cc
)

ecbuild_generate_yy( YYPREFIX    request_
                     YACC        parser/requesty
                     YACC_TARGET requesty
                     LEX         parser/requestl
                     LEX_TARGET  requestl
                     DEPENDANT   parser/RequestParser.cc )

ecbuild_add_library( TARGET             eckit_ecml
                     INSTALL_HEADERS    ALL
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/experimental/eckit/ecml
                     SOURCES            ${eckit_ecml_srcs}
                     LIBS               eckit )
