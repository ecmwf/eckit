list( APPEND eckit_ecml_srcs
# Parser
parser/Cell.h
parser/Cell.cc
parser/List.h
parser/List.cc
parser/Request.h
parser/Request.cc
parser/RequestParser.h
parser/RequestParser.cc

# AST
ast/FunctionDefinition.h
ast/FunctionDefinition.cc
ast/Closure.cc
ast/Closure.h

# Core language
core/Environment.h
core/Environment.cc
core/ExecutionContext.h
core/ExecutionContext.cc
core/Interpreter.h
core/Interpreter.cc
core/Module.cc
core/Module.h
core/RequestHandler.h
core/RequestHandler.cc
core/SpecialFormHandler.h
core/SpecialFormHandler.cc

# Prelude
prelude/Prelude.cc
prelude/Prelude.h
prelude/LetHandler.cc
prelude/LetHandler.h
prelude/VariableLookupHandler.cc
prelude/VariableLookupHandler.h
prelude/DefineFunctionHandler.cc
prelude/DefineFunctionHandler.h
prelude/ClosureHandler.h
prelude/ClosureHandler.cc
prelude/ListHandler.cc
prelude/ListHandler.h
prelude/PrintHandler.cc
prelude/PrintHandler.h
prelude/TestHandler.cc
prelude/TestHandler.h
prelude/FirstHandler.cc
prelude/FirstHandler.h
prelude/RestHandler.cc
prelude/RestHandler.h
prelude/IfHandler.cc
prelude/IfHandler.h
prelude/TemporaryFileHandler.h
prelude/TemporaryFileHandler.cc

# Data types' support
data/DataHandleFactory.h
data/DataHandleFactory.cc
data/FileHandleFactory.h
data/FileHandleFactory.cc
data/PartFileHandleFactory.h
data/PartFileHandleFactory.cc

# Miscellaneous
# Adapter for eckit::Parametrisation
misc/DynamicParametrisation.h
misc/DynamicParametrisation.cc
misc/ParameterizedRequestHandler.h
misc/ParameterizedRequestHandler.cc
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

ecbuild_add_executable( TARGET ecml_test
                        SOURCES tests/ecml_test.cc
                        LIBS eckit eckit_ecml )

ecbuild_add_executable( TARGET ecml_unittests
                        SOURCES tests/ecml_unittests.cc
                        LIBS eckit eckit_ecml )

add_subdirectory( tests )

