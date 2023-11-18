# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

ecbuild_add_test(
    TARGET  eckit_test_codec_encoding
    SOURCES test_codec_encoding.cc
    LIBS    eckit_codec
)

ecbuild_add_test(
    TARGET  eckit_test_codec_stream
    SOURCES test_codec_stream.cc
    LIBS    eckit_codec
)

ecbuild_add_executable(
    TARGET  eckit_test_codec_record
    SOURCES test_codec_record.cc
    LIBS    eckit_codec
    NOINSTALL
)

foreach( algorithm none bzip2 aec lz4 snappy )
    string( TOUPPER ${algorithm} feature )
    if( eckit_HAVE_${feature} OR algorithm MATCHES "none" )
        ecbuild_add_test(
            TARGET      eckit_test_codec_record_COMPRESSION_${algorithm}
            COMMAND     eckit_test_codec_record
            ARGS        --suffix ".${algorithm}"
            ENVIRONMENT ECKIT_CODEC_COMPRESSION=${algorithm}
        )
    endif()
endforeach()

