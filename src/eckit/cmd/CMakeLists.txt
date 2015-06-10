list( APPEND eckit_cmd_srcs
cmdlib.h
AliasCmd.cc
AliasCmd.h
Arg.cc
Arg.h
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
JSONCmd.cc
JSONCmd.h
HistoryCmd.cc
HistoryCmd.h
KillCmd.cc
KillCmd.h
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
RemoteCommandable.cc
RemoteCommandable.h
RemoteCommander.cc
RemoteCommander.h
RemoteCommandUser.cc
RemoteCommandUser.h
SleepCmd.cc
SleepCmd.h
StartCmd.cc
StartCmd.h
StatusCmd.cc
StatusCmd.h
TermBuf.cc
TermBuf.h
term.c
)

ecbuild_generate_yy( YYPREFIX eckit_cmd_
                     YACC cmdsy
                     LEX  cmdsl
                     DEPENDANT CmdParser.cc )

ecbuild_add_library( TARGET             eckit_cmd
                     INSTALL_HEADERS    LISTED
                     SOURCES            ${eckit_cmd_srcs}
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/cmd
                     INCLUDES           ${CURSES_INCLUDE_DIR}
                     LIBS               eckit ${CURSES_LIBRARIES} )
