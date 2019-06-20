/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLOutputConfig.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/sql/SQLSimpleOutput.h"

#include "eckit/os/BackTrace.h"

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

SQLOutputConfig::SQLOutputConfig(bool noColumnNames, bool noNULL, const std::string& delimiter,
                                 const std::string& format, bool bitfieldsBinary, bool noColumnAlignment,
                                 bool fullPrecision) :
    outputFile_(""),
    doNotWriteColumnNames_(noColumnNames),
    fieldDelimiter_(delimiter),
    outputFormat_(format),
    displayBitfieldsBinary_(bitfieldsBinary),
    disableAlignmentOfColumns_(noColumnAlignment),
    fullPrecision_(fullPrecision),
    doNotWriteNULL_(noNULL) {}

SQLOutputConfig::~SQLOutputConfig() {}

void SQLOutputConfig::setOutputFile(const eckit::PathName& filename) {
    outputFile_ = filename;
}

SQLOutput* SQLOutputConfig::buildOutput() const {
    return buildOutput(outputFile_);
}

SQLOutput* SQLOutputConfig::buildOutput(const eckit::PathName& path) const {
    // Output file not supported in base config
    ASSERT(path.asString().empty());
    if (outputFormat_ != "default" && outputFormat_ != "wide") {
        throw UserError("Unsupported output format: " + outputFormat_, Here());
    }
    return new SQLSimpleOutput(*this, std::cout);
}

const std::string& SQLOutputConfig::fieldDelimiter() const {
    return fieldDelimiter_;
}

const std::string& SQLOutputConfig::outputFormat() const {
    return outputFormat_;
}

bool SQLOutputConfig::doNotWriteNULL() const {
    return doNotWriteNULL_;
}

bool SQLOutputConfig::fullPrecision() const {
    return fullPrecision_;
}

bool SQLOutputConfig::displayBitfieldsBinary() const {
    return displayBitfieldsBinary_;
}

bool SQLOutputConfig::disableAlignmentOfColumns() const {
    return disableAlignmentOfColumns_;
}

bool SQLOutputConfig::doNotWriteColumnNames() const {
    return doNotWriteColumnNames_;
}


const char* SQLOutputConfig::defaultDelimiter = "	";
// const char* SQLOutputConfig::defaultOutputFile = "output.odb";
const char* SQLOutputConfig::defaultOutputFormat = "default";

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit
