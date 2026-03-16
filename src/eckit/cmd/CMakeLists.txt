list( APPEND eckit_cmd_srcs
    AliasCmd.cc
    AliasCmd.h
    Arg.cc
    Arg.h
    CmdApplication.cc
    CmdApplication.h
    CmdArg.cc
    CmdArg.h
    CmdParser.cc
    CmdParser.h
    CmdResource.cc
    CmdResource.h
    ConfigCmd.cc
    ConfigCmd.h
    DirCmd.cc
    DirCmd.h
    EchoCmd.cc
    EchoCmd.h
    EnvironmentCmd.cc
    EnvironmentCmd.h
    ExportCmd.cc
    ExportCmd.h
    HistoryCmd.cc
    HistoryCmd.h
    JSONCmd.cc
    JSONCmd.h
    KillCmd.cc
    KillCmd.h
    LibEcKitCmd.cc
    LibEcKitCmd.h
    LockCmd.cc
    LockCmd.h
    ManCmd.cc
    ManCmd.h
    MemoryCmd.cc
    MemoryCmd.h
    PsCmd.cc
    PsCmd.h
    QuitCmd.cc
    QuitCmd.h
    RemoteCmd.cc
    RemoteCmd.h
    RemoteCommandUser.cc
    RemoteCommandUser.h
    RemoteCommandable.cc
    RemoteCommandable.h
    RemoteCommander.cc
    RemoteCommander.h
    SleepCmd.cc
    SleepCmd.h
    StartCmd.cc
    StartCmd.h
    StatusCmd.cc
    StatusCmd.h
    StopCmd.cc
    StopCmd.h
    TailCmd.cc
    TailCmd.h
    TermBuf.cc
    TermBuf.h
    UpTimeCmd.cc
    UpTimeCmd.h
    UserInput.cc
    UserInput.h
    term.c
)

ecbuild_generate_yy(
    YYPREFIX eckit_cmd_
    YACC cmdsy
    LEX  cmdsl
    DEPENDANT CmdParser.cc
)

# Supress warnings emited from YACC generated code
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND suppress_warnings
        -Wno-unused-function
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND suppress_warnings
        -Wno-unused-function
        -Wno-unneeded-internal-declaration
    )
endif()
set_source_files_properties(
    CmdParser.cc
    PROPERTIES
        COMPILE_OPTIONS "${suppress_warnings}"
)

ecbuild_add_library( TARGET             eckit_cmd TYPE SHARED
                     INSTALL_HEADERS    LISTED
                     SOURCES            ${eckit_cmd_srcs}
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/cmd
                     PRIVATE_INCLUDES   ${CURSES_INCLUDE_DIR}
                     PRIVATE_LIBS       ${CURSES_LIBRARIES}
                     PUBLIC_LIBS        eckit )
