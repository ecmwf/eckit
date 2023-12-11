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


#include "eckit/codec/RecordPrinter.h"

#include <sstream>

#include "eckit/codec/Exceptions.h"
#include "eckit/codec/FileStream.h"
#include "eckit/codec/print/JSONFormat.h"
#include "eckit/codec/print/TableFormat.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

RecordPrinter::RecordPrinter(const PathName& path, const Configuration& config) : RecordPrinter(path, 0, config) {}

//---------------------------------------------------------------------------------------------------------------------

RecordPrinter::RecordPrinter(const PathName& path, const std::uint64_t offset, const Configuration& config) :
    RecordPrinter(Record::URI{path, offset}, config) {}

//---------------------------------------------------------------------------------------------------------------------

RecordPrinter::RecordPrinter(const Record::URI& ref, const Configuration& config) :
    uri_(ref), record_(Session::record(ref.path, ref.offset)) {
    if (record_.empty()) {
        auto in = InputFileStream(uri_.path);
        in.seek(uri_.offset);
        record_.read(in, true);
        ASSERT(not record_.empty());
    }

    config.get("format", options_.format);
    config.get("details", options_.details);

    // Check if format is supported
    {
        std::vector<std::string> supported_formats{"json", "yaml", "table"};
        bool format_supported{false};
        for (auto& supported_format : supported_formats) {
            if (options_.format == supported_format) {
                format_supported = true;
                break;
            }
        }
        if (not format_supported) {
            std::stringstream s;
            s << "Format '" + options_.format + "' not supported. Supported formats:";
            for (auto& supported_format : supported_formats) {
                s << "\n  - " << supported_format;
            }
            throw Exception(s.str(), Here());
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------

void RecordPrinter::print(std::ostream& out) const {
    LocalConfiguration config;
    config.set("details", options_.details);
    if (options_.format == "json") {
        JSONFormat{uri_, config}.print(out);
    }
    else if (options_.format == "yaml") {
        JSONFormat{uri_, config}.print(out);
    }
    else if (options_.format == "table") {
        TableFormat{uri_, config}.print(out);
    }
    else {
        throw Exception("Cannot print record: Unrecognized format " + options_.format + ".", Here());
    }
}

//---------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const RecordPrinter& info) {
    info.print(out);
    return out;
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
