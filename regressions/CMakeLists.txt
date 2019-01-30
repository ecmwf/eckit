list( APPEND regressions ECKIT-175 ECKIT-221 ECKIT-166 )

foreach( r ${regressions} )

  configure_file( ${r}.sh.in ${r}.sh @ONLY )

  ecbuild_add_executable(
    NOINSTALL
    TARGET   ${r}.x
    SOURCES  ${r}.cc
    LIBS     eckit_mpi eckit )

  ecbuild_add_test(
    TYPE     SCRIPT
    COMMAND  ${r}.sh )

endforeach()
