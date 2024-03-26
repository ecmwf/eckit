
configure_file( eckit_codec_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/eckit_codec_config.h @ONLY )
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/eckit_codec_config.h
    DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/codec
)

ecbuild_add_library(
    TARGET eckit_codec

    INSTALL_HEADERS    ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/codec
    PUBLIC_LIBS        eckit
    PUBLIC_INCLUDES
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
      $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>

    SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/eckit_codec_config.h
        Data.cc
        Data.h
        Exceptions.cc
        Exceptions.h
        FileStream.cc
        FileStream.h
        Metadata.cc
        Metadata.h
        ReadRequest.cc
        ReadRequest.h
        Record.cc
        Record.h
        RecordItem.cc
        RecordItem.h
        RecordItemReader.cc
        RecordItemReader.h
        RecordPrinter.cc
        RecordPrinter.h
        RecordReader.cc
        RecordReader.h
        RecordWriter.cc
        RecordWriter.h
        Session.cc
        Session.h
        Stream.cc
        Stream.h
        codec.h
        detail/Base64.cc
        detail/Base64.h
        detail/Checksum.cc
        detail/Checksum.h
        detail/DataInfo.h
        detail/DataType.cc
        detail/DataType.h
        detail/Decoder.cc
        detail/Decoder.h
        detail/Defaults.h
        detail/demangle.cc
        detail/demangle.h
        detail/Encoder.cc
        detail/Encoder.h
        detail/Endian.h
        detail/Link.cc
        detail/Link.h
        detail/NoConfig.h
        detail/ParsedRecord.h
        detail/RecordInfo.h
        detail/RecordSections.h
        detail/Reference.h
        detail/StaticAssert.h
        detail/Time.cc
        detail/Time.h
        detail/Type.h
        detail/TypeTraits.h
        detail/Version.h
        detail/sfinae.h
        detail/tag.h
        print/Bytes.cc
        print/Bytes.h
        print/JSONFormat.cc
        print/JSONFormat.h
        print/TableFormat.cc
        print/TableFormat.h
        types/array.h
        types/array/ArrayMetadata.cc
        types/array/ArrayMetadata.h
        types/array/ArrayReference.cc
        types/array/ArrayReference.h
        types/array/adaptors/StdArrayAdaptor.h
        types/array/adaptors/StdVectorAdaptor.h
        types/array/adaptors/StdVectorOfStdArrayAdaptor.h
        types/scalar.cc
        types/scalar.h
        types/string.h
)

target_compile_features( eckit_codec PUBLIC cxx_std_17 )
