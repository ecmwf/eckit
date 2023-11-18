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


#include "eckit/codec/RecordWriter.h"

#include "eckit/codec/Exceptions.h"
#include "eckit/codec/detail/Checksum.h"
#include "eckit/codec/detail/Defaults.h"
#include "eckit/codec/detail/Encoder.h"
#include "eckit/codec/detail/RecordSections.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

template <typename OStream, typename Struct>
inline void write_struct(OStream& out, const Struct& s) {
    static_assert(Struct::bytes == sizeof(Struct));
    if (out.write(reinterpret_cast<const char*>(&s), sizeof(s)) != sizeof(s)) {
        throw WriteError("Could not write struct to stream");
    }
}

//---------------------------------------------------------------------------------------------------------------------

template <typename OStream>
inline void write_string(OStream& out, const std::string& s) {
    if (out.write(s.data(), s.size()) != s.size()) {
        throw WriteError("Could not write string to stream");
    }
}

//---------------------------------------------------------------------------------------------------------------------

size_t RecordWriter::write(Stream out) const {
    RecordHead r;

    auto begin_of_record = out.position();

    auto position = [&begin_of_record, &out]() { return out.position() - begin_of_record; };

    std::vector<RecordDataIndexSection::Entry> index;

    // Begin Record
    // ------------
    write_struct(out, r);

    // Metadata section
    // ----------------
    {
        r.metadata_offset = position();
        write_struct(out, RecordMetadataSection::Begin());
        auto metadata_str = metadata();
        write_string(out, metadata_str);
        write_struct(out, RecordMetadataSection::End());
        r.metadata_length = position() - r.metadata_offset;
        r.metadata_checksum
            = do_checksum_ != 0 ? codec::checksum(metadata_str.data(), metadata_str.size()) : std::string("none:");

        // Index section
        // -------------
        auto nb_data_sections = static_cast<size_t>(nb_data_sections_);
        r.index_offset        = position();
        write_struct(out, RecordDataIndexSection::Begin());
        index.resize(nb_data_sections);
        for (size_t i = 0; i < nb_data_sections; ++i) {
            write_struct(out, index[i]);
        }
        write_struct(out, RecordDataIndexSection::End());
        r.index_length = position() - r.index_offset;
    }

    // Data sections
    // -------------
    {
        size_t i{0};
        for (const auto& key : keys_) {
            const auto& encoder = encoders_.at(key);
            const auto& info    = info_.at(key);
            if (info.section() == 0) {
                continue;
            }
            Data data;
            encode_data(encoder, data);
            data.compress(info.compression());
            auto& data_section  = index[i];
            data_section.offset = position();
            write_struct(out, RecordDataSection::Begin());
            if (data.write(out) != data.size()) {
                throw WriteError("Could not write data for item " + key + " to stream");
            }
            write_struct(out, RecordDataSection::End());
            data_section.length   = position() - data_section.offset;
            data_section.checksum = do_checksum_ != 0 ? data.checksum() : std::string("none:");
            ++i;
        }
    }

    // End Record
    // ----------
    write_struct(out, RecordEnd());
    auto end_of_record = out.position();

    r.record_length = end_of_record - begin_of_record;
    r.time          = Time::now();

    out.seek(begin_of_record);
    write_struct(out, r);

    out.seek(begin_of_record + r.index_offset + sizeof(RecordDataIndexSection::Begin));
    for (auto& entry : index) {
        write_struct(out, entry);
    }
    out.seek(end_of_record);  // So that following writes will not overwrite
    return r.record_length;
}

//---------------------------------------------------------------------------------------------------------------------

void RecordWriter::compression(const std::string& c) {
    compression_ = c;
}

//---------------------------------------------------------------------------------------------------------------------

void RecordWriter::compression(bool on) {
    if (on) {
        compression_ = defaults::compression_algorithm();  // still possible to be "none"
    }
    else {
        compression_ = "none";
    }
}

//---------------------------------------------------------------------------------------------------------------------

void RecordWriter::checksum(bool on) {
    do_checksum_ = on && defaults::checksum_write() ? 1 : 0;  //  possible to be off via environment
}

//---------------------------------------------------------------------------------------------------------------------

void RecordWriter::set(const RecordWriter::Key& key, Link&& link, const Configuration&) {
    keys_.emplace_back(key);
    encoders_[key] = Encoder{link};
    info_.emplace(key, DataInfo{});
}

//---------------------------------------------------------------------------------------------------------------------

void RecordWriter::set(const RecordWriter::Key& key, Encoder&& encoder, const Configuration& config) {
    DataInfo info;
    if (encoder.encodes_data()) {
        ++nb_data_sections_;
        info.compression(config.getString("compression", compression_));
        info.section(nb_data_sections_);
    }
    keys_.emplace_back(key);
    encoders_[key] = std::move(encoder);
    info_.emplace(key, std::move(info));
}

//---------------------------------------------------------------------------------------------------------------------

size_t RecordWriter::write(const PathName& path, Mode mode) const {
    return write(OutputFileStream(path, mode));
}

//---------------------------------------------------------------------------------------------------------------------

size_t RecordWriter::write(DataHandle& out) const {
    return write(Stream(out));
}

//---------------------------------------------------------------------------------------------------------------------

size_t RecordWriter::estimateMaximumSize() const {
    size_t size{0};

    size += sizeof(RecordHead);

    size += sizeof(RecordMetadataSection::Begin);
    size += metadata().size();
    size += sizeof(RecordMetadataSection::End);

    size += sizeof(RecordDataIndexSection::Begin);
    size += static_cast<size_t>(nb_data_sections_) * sizeof(RecordDataIndexSection::Entry);
    size += sizeof(RecordDataIndexSection::End);

    for (const auto& key : keys_) {
        const auto& encoder = encoders_.at(key);
        const auto& info    = info_.at(key);
        if (info.section() == 0) {
            continue;
        }
        size += sizeof(RecordDataSection::Begin);
        {
            Metadata m;
            size_t max_data_size = encode_metadata(encoder, m);
            if (info.compression() != "none") {
                max_data_size = static_cast<size_t>(1.2 * static_cast<double>(max_data_size));
                max_data_size = std::max<size_t>(max_data_size, 10 * 1024);  // minimum 10KB
            }
            size += max_data_size;
        }
        size += sizeof(RecordDataSection::End);
    }

    size += sizeof(RecordEnd);

    return size;
}

//---------------------------------------------------------------------------------------------------------------------

std::string RecordWriter::metadata() const {
    Metadata metadata;
    for (const auto& key : keys_) {
        const auto& encoder = encoders_.at(key);
        const auto& info    = info_.at(key);
        Metadata m;
        encode_metadata(encoder, m);
        if (info.section() != 0) {
            m.set("data.section", info.section());
            if (info.compression() != "none") {
                m.set("data.compression.type", info.compression());
            }
        }
        metadata.set(key, m);
    }
    std::stringstream ss;
    codec::write(metadata, ss);
    return ss.str();
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
