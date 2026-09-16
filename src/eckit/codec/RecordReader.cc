// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/codec/RecordReader.h"

#include "eckit/codec/Metadata.h"
#include "eckit/codec/RecordItemReader.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

RecordReader::RecordReader(const Record::URI& ref) : RecordReader(ref.path, ref.offset) {}

//---------------------------------------------------------------------------------------------------------------------

RecordReader::RecordReader(const std::string& path, uint64_t offset) : path_{path}, offset_{offset} {}

RecordReader::RecordReader(Stream stream, uint64_t offset) : stream_{stream}, offset_{offset} {}

//---------------------------------------------------------------------------------------------------------------------

Record::URI RecordReader::uri() const {
    Record::URI _uri;
    _uri.path   = path_;
    _uri.offset = offset_;
    return _uri;
}

//---------------------------------------------------------------------------------------------------------------------

RecordItem::URI RecordReader::uri(const std::string& key) const {
    return {path_, offset_, key};
}

//---------------------------------------------------------------------------------------------------------------------

void RecordReader::trace(const std::string&, const char* file, int line, const char* func) {}

//---------------------------------------------------------------------------------------------------------------------

void RecordReader::wait(const std::string& key) {
    request(key).wait();
}

//---------------------------------------------------------------------------------------------------------------------

void RecordReader::wait() {
    // This can be optimized perhaps to overlap IO with decoding in multithreaded environment
    for (auto& pair : requests_) {
        auto& request = pair.second;
        request.wait();
    }
}

//---------------------------------------------------------------------------------------------------------------------

ReadRequest& RecordReader::request(const std::string& key) {
    return requests_.at(key);
}

//---------------------------------------------------------------------------------------------------------------------

Metadata RecordReader::metadata(const std::string& key) {
    Metadata metadata;
    RecordItemReader{uri(key)}.read(metadata);
    return metadata;
}

void RecordReader::checksum(bool b) {
    do_checksum_ = b ? 1 : 0;
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
