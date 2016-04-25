find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

list( APPEND eckit_ecml_srcs
# Parser
parser/Cell.h
parser/Cell.cc
parser/CellPrinter.h
parser/CellPrinter.cc
parser/CellDotPrinter.h
parser/CellDotPrinter.cc
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
prelude/UpdateHandler.h
prelude/UpdateHandler.cc
prelude/VariableLookupHandler.cc
prelude/VariableLookupHandler.h
prelude/DefineFunctionHandler.cc
prelude/DefineFunctionHandler.h
prelude/ApplyHandler.cc
prelude/ApplyHandler.h
prelude/ClosureHandler.h
prelude/ClosureHandler.cc
prelude/ListHandler.cc
prelude/ListHandler.h
prelude/SequenceHandler.cc
prelude/SequenceHandler.h
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
prelude/SystemHandler.h
prelude/SystemHandler.cc
prelude/GetenvHandler.h
prelude/GetenvHandler.cc
prelude/JoinStringsHandler.h
prelude/JoinStringsHandler.cc
prelude/QuoteHandler.h
prelude/QuoteHandler.cc
prelude/NullHandler.h
prelude/NullHandler.cc
prelude/RunHandler.h
prelude/RunHandler.cc
prelude/REPLHandler.h
prelude/REPLHandler.cc
prelude/RangeHandler.h
prelude/RangeHandler.cc
prelude/ForHandler.cc
prelude/ForHandler.h
prelude/GlobHandler.cc
prelude/GlobHandler.h
prelude/ReadTextFileHandler.h
prelude/ReadTextFileHandler.cc
prelude/TryHandler.h
prelude/TryHandler.cc
prelude/ThrowHandler.h
prelude/ThrowHandler.cc

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
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/ecml
                     SOURCES            ${eckit_ecml_srcs}
                     LIBS               eckit 
                                        eckit_cmd # for UserInput
                                        )

ecbuild_add_executable( TARGET eckit_ecml_test
                        SOURCES tests/ecml_test.cc
                        LIBS eckit eckit_ecml )

ecbuild_add_executable( TARGET eckit_ecml_unittests
                        SOURCES tests/ecml_unittests.cc
                        LIBS eckit eckit_ecml)

add_subdirectory( tests )

