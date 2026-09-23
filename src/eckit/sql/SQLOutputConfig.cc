// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/sql/SQLOutputConfig.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/os/BackTrace.h"
#include "eckit/sql/SQLSimpleOutput.h"

namespace eckit::sql {

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

}  // namespace eckit::sql
