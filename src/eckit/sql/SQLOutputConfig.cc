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

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

SQLOutputConfig::SQLOutputConfig(bool cn,
                bool n,
                const std::string& d,
                const std::string& output,
                const std::string& format,
                bool displayBitfieldsBinary,
                bool displayBitfieldsHexadecimal,                    
                bool disableAlignmentOfColumns,
                bool fullPrecision)
: doNotWriteColumnNames_(cn),
  doNotWriteNULL_(n),
  fieldDelimiter_(d),
  outputFile_(output),
  outputFormat_(format),
  displayBitfieldsBinary_(displayBitfieldsBinary),
  displayBitfieldsHexadecimal_(displayBitfieldsHexadecimal),
  disableAlignmentOfColumns_(disableAlignmentOfColumns),
  fullPrecision_(fullPrecision)
{}

bool SQLOutputConfig::doNotWriteColumnNames () const { return doNotWriteColumnNames_; }
void SQLOutputConfig::doNotWriteColumnNames(bool b) { doNotWriteColumnNames_ = b; }

bool SQLOutputConfig::doNotWriteNULL () const { return  doNotWriteNULL_; }
void SQLOutputConfig::doNotWriteNULL (bool b) { doNotWriteNULL_ = b; }

const std::string& SQLOutputConfig::fieldDelimiter() const { return fieldDelimiter_; }
void SQLOutputConfig::fieldDelimiter(const std::string& d) { fieldDelimiter_ = d; }

const std::string& SQLOutputConfig::outputFile () const { return outputFile_; }
void SQLOutputConfig::outputFile (const std::string& fn) { outputFile_ = fn; }

const std::string& SQLOutputConfig::outputFormat () const { return outputFormat_; }
void SQLOutputConfig::outputFormat (const std::string& s) { outputFormat_ = s; }

bool SQLOutputConfig::displayBitfieldsBinary () const { return displayBitfieldsBinary_; }
void SQLOutputConfig::displayBitfieldsBinary (bool b) { displayBitfieldsBinary_ = b; }

bool SQLOutputConfig::displayBitfieldsHexadecimal () const { return displayBitfieldsHexadecimal_; }
void SQLOutputConfig::displayBitfieldsHexadecimal (bool b) { displayBitfieldsHexadecimal_ = b; }

bool SQLOutputConfig::disableAlignmentOfColumns () const { return disableAlignmentOfColumns_; }
void SQLOutputConfig::disableAlignmentOfColumns (bool b) { disableAlignmentOfColumns_ = b; }

bool SQLOutputConfig::fullPrecision() const { return fullPrecision_; }
void SQLOutputConfig::fullPrecision(bool b) { fullPrecision_ = b; }

const SQLOutputConfig SQLOutputConfig::defaultConfig() { return SQLOutputConfig(); }
const char* SQLOutputConfig::defaultDelimiter() { return "	"; }
const char* SQLOutputConfig::defaultOutputFile() { return "output.odb"; }
const char* SQLOutputConfig::defaultFormat() { return "default"; }

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace eckit

