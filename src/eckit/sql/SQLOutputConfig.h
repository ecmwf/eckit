/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLOutputConfig.h
/// Piotr Kuchta - ECMWF Jul 2010

#ifndef odb_api_SQLOutputConfig_H
#define odb_api_SQLOutputConfig_H

#include <string>

namespace eckit {
namespace sql {

class SQLOutputConfig {
public:

	SQLOutputConfig(const SQLOutputConfig&);
	SQLOutputConfig& operator=(const SQLOutputConfig&);
	SQLOutputConfig(bool cn = false,
                    bool n = false,
                    const std::string& d = defaultDelimiter(),
                    const std::string& output = defaultOutputFile(),
                    const std::string& format = defaultFormat(),
                    bool displayBitfieldsBinary = false,
                    bool displayBitfieldsHexadecimal = false,                    
                    bool disableAlignmentOfColumns = false,
                    bool fullPrecision = false);
	bool doNotWriteColumnNames () const;
    void doNotWriteColumnNames(bool b);

	bool doNotWriteNULL () const;
	void doNotWriteNULL (bool b);

    const std::string& fieldDelimiter() const;
    void fieldDelimiter(const std::string& d);

    const std::string& outputFile () const;
    void outputFile (const std::string& fn);

    const std::string& outputFormat () const;
    void outputFormat (const std::string& s);

    bool displayBitfieldsBinary () const;
    void displayBitfieldsBinary (bool b);

    bool displayBitfieldsHexadecimal () const;
    void displayBitfieldsHexadecimal (bool b);
    
    bool disableAlignmentOfColumns () const;
    void disableAlignmentOfColumns (bool b);

    bool fullPrecision () const;
    void fullPrecision (bool);

	static const SQLOutputConfig defaultConfig();

private:
	bool doNotWriteColumnNames_;
	bool doNotWriteNULL_;
    std::string fieldDelimiter_;
    std::string outputFile_;          // -o
    std::string outputFormat_;        // -f
    bool displayBitfieldsBinary_;     // --binary
    bool displayBitfieldsHexadecimal_; // --hex
    bool disableAlignmentOfColumns_;  // --no_alignment
    bool fullPrecision_;              // --full_precision

    static const char* defaultDelimiter();
    static const char* defaultOutputFile();
    static const char* defaultFormat();
};

} // namespace sql
} // namespace eckit

#endif
