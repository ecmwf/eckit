### test data files
list( APPEND test_data_files
2t_sfc.grib
2t_sfc_regrid.grib
vo-d_6ml.grib
u-v_6ml.grib
q_6ml_regrid.grib
)

ecbuild_get_test_multidata( TARGET get_eckit_test_data
#                            DIRNAME eckit
                            NAMES ${test_data_files}
                            NOCHECK )

ecbuild_add_test( TARGET      eckit_test_utils_string_tools
                  SOURCES     test_string_tools.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_translator
                  SOURCES     test_translator.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_tokenizer
                  SOURCES     test_tokenizer.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_rendezvoushash
                  SOURCES     test_rendezvoushash.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_compressor
                  SOURCES     test_compressor.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_hashing
                  CONDITION   ECKIT_HAVE_SSL
                  INCLUDES    "${OPENSSL_INCLUDE_DIR}"
                  SOURCES     test_hashing.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_hash_performance
                  CONDITION   HAVE_EXTRA_TESTS
                  SOURCES     hash-performance.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils_compression_performance
                  CONDITION   HAVE_EXTRA_TESTS
                  TEST_DEPENDS get_eckit_test_data
                  SOURCES     compression-performance.cc
                  LIBS        eckit )
