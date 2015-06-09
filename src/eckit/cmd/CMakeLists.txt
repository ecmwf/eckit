list( APPEND eckit_cmd_srcs
cmdlib.h
Arg.cc
Arg.h
CmdArg.cc
CmdArg.h
CmdResource.cc
CmdResource.h
ConfigCmd.cc
ConfigCmd.h
JSONCmd.cc
JSONCmd.h
MemoryCmd.cc
MemoryCmd.h
RemoteCommand.cc
RemoteCommand.h
StatusCmd.cc
StatusCmd.h
TermBuf.cc
TermBuf.h
term.c
)

ecbuild_add_library( TARGET             eckit_cmd
                     INSTALL_HEADERS    LISTED
                     SOURCES            ${eckit_cmd_srcs}
                     HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/cmd
                     INCLUDES           ${CURSES_INCLUDE_DIR}
                     LIBS               eckit ${CURSES_LIBRARIES} )
