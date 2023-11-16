/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/codec/RecordItemReader.h"

#include "eckit/codec/Exceptions.h"
#include "eckit/codec/FileStream.h"
#include "eckit/codec/Record.h"
#include "eckit/codec/Session.h"
#include "eckit/codec/detail/ParsedRecord.h"
#include "eckit/codec/detail/RecordSections.h"

namespace eckit::codec {


namespace {

//---------------------------------------------------------------------------------------------------------------------

template <typename IStream, typename Struct>
inline size_t read_struct(IStream& in, Struct& s) {
    static_assert(Struct::bytes == sizeof(Struct));
    return in.read(reinterpret_cast<char*>(&s), sizeof(Struct));
}

//---------------------------------------------------------------------------------------------------------------------

template <typename Struct, typename IStream>
inline Struct read_struct(IStream& in) {
    Struct s;
    if (read_struct(in, s) != sizeof(Struct)) {
        throw InvalidRecord("Unexpected EOF reached");
    }
    return s;
}

//---------------------------------------------------------------------------------------------------------------------

Data read_data(const Record& record, int data_section_index, Stream in) {
    if (data_section_index == 0) {
        return {};
    }

    const auto& parsed       = static_cast<const ParsedRecord&>(record);
    const auto& data_section = parsed.data_sections.at(static_cast<size_t>(data_section_index) - 1);

    Data data;
    auto offset = data_section.offset;
    in.seek(offset);

    auto data_begin = read_struct<RecordDataSection::Begin>(in);
    if (not data_begin.valid()) {
        throw InvalidRecord("Data section is not valid");
    }
    auto data_size
        = static_cast<size_t>(data_section.length) - sizeof(RecordDataSection::Begin) - sizeof(RecordDataSection::End);
    if (data_size > 0) {
        if (data.read(in, data_size) != data_size) {
            throw InvalidRecord("Data section is not valid");
        }
        ASSERT(data.size() == data_size);
    }
    auto data_end = read_struct<RecordDataSection::End>(in);
    if (not data_end.valid()) {
        throw InvalidRecord("Data section is not valid");
    }
    return data;
}

//---------------------------------------------------------------------------------------------------------------------

PathName make_absolute_path(const std::string& reference_path, RecordItem::URI& uri) {
    PathName absolute_path = uri.path;
    if (!reference_path.empty() && uri.path[0] != '/' && uri.path[0] != '~') {
        absolute_path = PathName{reference_path} / absolute_path;
    }
    return absolute_path.fullName();
}

//---------------------------------------------------------------------------------------------------------------------

Record read_record(const std::string& path, size_t offset) {
    auto record = Session::record(path, offset);
    if (record.empty()) {
        auto in = InputFileStream(path);
        in.seek(offset);
        record.read(in);
    }
    return record;
}

//---------------------------------------------------------------------------------------------------------------------

Record read_record(Stream in, size_t offset) {
    auto record = Session::record(in, offset);
    if (record.empty()) {
        in.seek(offset);
        record.read(in);
    }
    return record;
}

//---------------------------------------------------------------------------------------------------------------------

}  // anonymous namespace

//---------------------------------------------------------------------------------------------------------------------

RecordItemReader::RecordItemReader(Stream in, size_t offset, const std::string& key) : in_(in), uri_{"", offset, key} {
    record_ = read_record(in, uri_.offset);

    if (not record_.has(uri_.key)) {
        throw InvalidRecord(uri_.key + " not found in record " + uri_.path);
    }
}

RecordItemReader::RecordItemReader(Stream in, const std::string& key) : in_(in), uri_{"", 0, key} {
    record_ = read_record(in, uri_.offset);

    if (not record_.has(uri_.key)) {
        throw InvalidRecord(uri_.key + " not found in record " + uri_.path);
    }
}


//---------------------------------------------------------------------------------------------------------------------

RecordItemReader::RecordItemReader(const std::string& uri) : RecordItemReader("", uri) {}

//---------------------------------------------------------------------------------------------------------------------

RecordItemReader::RecordItemReader(const std::string& ref, const std::string& uri) : ref_{ref}, uri_{uri} {
    auto absolute_path = make_absolute_path(ref_, uri_);

    if (not absolute_path.exists()) {
        throw InvalidRecord("Item " + uri_.str() + " refers to non existing file: " + absolute_path);
    }
    record_ = read_record(absolute_path, uri_.offset);

    if (not record_.has(uri_.key)) {
        throw InvalidRecord(uri_.key + " not found in record " + uri_.path);
    }
}

//---------------------------------------------------------------------------------------------------------------------

void RecordItemReader::read(RecordItem& item) {
    Metadata metadata;
    Data data;

    read(metadata, data);

    item.metadata(metadata);
    item.data(std::move(data));
};

//---------------------------------------------------------------------------------------------------------------------

void RecordItemReader::read(Metadata& metadata, bool follow_links) {
    metadata = record_.metadata(uri_.key);

    if (follow_links && metadata.link()) {
        auto absolute_path = make_absolute_path(ref_, uri_);

        Metadata linked;
        RecordItemReader{absolute_path.dirName(), metadata.link()}.read(linked);
        metadata.link(std::move(linked));
    }
};

//---------------------------------------------------------------------------------------------------------------------

static void read_from_stream(Record record, Stream in, const std::string& key, Metadata& metadata, Data& data) {
    metadata = record.metadata(key);

    if (metadata.link()) {
        throw Exception("Cannot follow links in records that are not file based");
    }

    if (metadata.data.section() != 0) {
        data = read_data(record, metadata.data.section(), in);
    }
}


void RecordItemReader::read(Metadata& metadata, Data& data) {
    if (in_) {
        read_from_stream(record_, in_, uri_.key, metadata, data);
        return;
    }

    metadata = record_.metadata(uri_.key);

    auto absolute_path = make_absolute_path(ref_, uri_);

    if (metadata.link()) {
        Metadata linked;
        RecordItemReader{absolute_path.dirName(), metadata.link()}.read(linked, data);
        metadata.link(std::move(linked));
    }
    else if (metadata.data.section() != 0) {
        data = read_data(record_, metadata.data.section(), InputFileStream(absolute_path));
    }
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
