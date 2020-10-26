ecbuild_add_library( TARGET     eckit_test_value_custom_params TYPE SHARED
                     NOINSTALL
                     SOURCES    AnyKeyParams.cc
                     PUBLIC_LIBS eckit )
if( ECBUILD_2_COMPAT )
  list( REMOVE_ITEM ${PROJECT_NAME}_ALL_LIBS eckit_test_value_custom_params )
  set( ${PROJECT_NAME}_ALL_LIBS ${${PROJECT_NAME}_ALL_LIBS} CACHE INTERNAL "" )
endif()

ecbuild_add_test( TARGET   eckit_test_value_value_params
                  SOURCES  test_value_params.cc
                  LIBS     eckit eckit_test_value_custom_params )

ecbuild_add_test( TARGET   eckit_test_value_value_properties
                  SOURCES  test_value_properties.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_value_value_const
                  SOURCES  test_value_const.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_value_value_typeordering
                  SOURCES  test_value_typeordering.cc
                  LIBS     eckit )

foreach( valueType nil boolean integer double string valuemap valuelist date orderedmap )
   ecbuild_add_test( TARGET   eckit_test_value_value_${valueType}
                     SOURCES  test_value_${valueType}.cc
                     LIBS     eckit )
endforeach()

if( CMAKE_CXX_COMPILER_ID MATCHES "Cray" )
   # Disable warnings for test_value_integer due to following:
   #    "Integer conversion resulted in a change of sign."
   set_source_files_properties(test_value_integer.cc PROPERTIES COMPILE_FLAGS "-hmsglevel_4" )
endif()
