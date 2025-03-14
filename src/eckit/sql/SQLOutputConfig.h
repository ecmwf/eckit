/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// ECMWF Jul 2010

#ifndef eckit_sql_SQLOutputConfig_H
#define eckit_sql_SQLOutputConfig_H

#include "eckit/filesystem/PathName.h"

namespace eckit::sql {

class SQLOutput;

//----------------------------------------------------------------------------------------------------------------------

// SQLOutputConfig provides a mechanism for an application to construct output functionality
// according to its requirements:
//
//   i) User specification (e.g. command line options)
//  ii) Application requirements (e.g. output file format)
// iii) SQL statements (i.e. INTO, which cannot be specified before the SQL command is run).

class SQLOutputConfig {

public:  // methods

    SQLOutputConfig(bool noColumnNames = false, bool noNULL = false, const std::string& delimiter = defaultDelimiter,
                    const std::string& format = defaultOutputFormat, bool bitfieldsBinary = false,
                    bool noColumnAlignment = false, bool fullPrecision = false);

    virtual ~SQLOutputConfig();

    SQLOutput* buildOutput() const;
    virtual SQLOutput* buildOutput(const eckit::PathName& path) const;

    void setOutputFile(const eckit::PathName& filename);

    // Standard format properties (may not be applicable to all output types, but needed for
    // compatibility with SQLSimpleOutput).

    const std::string& fieldDelimiter() const;
    const std::string& outputFormat() const;
    bool doNotWriteNULL() const;
    bool fullPrecision() const;
    bool displayBitfieldsBinary() const;
    bool disableAlignmentOfColumns() const;
    bool doNotWriteColumnNames() const;

    // Defaults!

    static const char* defaultDelimiter;
    static const char* defaultOutputFormat;

protected:  // members

    eckit::PathName outputFile_;

    bool doNotWriteColumnNames_;
    std::string fieldDelimiter_;
    std::string outputFormat_;
    bool displayBitfieldsBinary_;     // --binary
    bool disableAlignmentOfColumns_;  // --no_alignment
    bool fullPrecision_;              // --full_precision
    bool doNotWriteNULL_;
};


// TODO: Move into odc
// 	SQLOutputConfig(bool cn = false,
//                     bool n = false,
//                     const std::string& d = defaultDelimiter(),
//                     const std::string& output = defaultOutputFile(),
//                     const std::string& format = defaultFormat(),
//                     bool displayBitfieldsBinary = false,
//                     bool displayBitfieldsHexadecimal = false,
//                     bool disableAlignmentOfColumns = false,
//                     bool fullPrecision = false);
//
// 	bool doNotWriteColumnNames () const;
//     void doNotWriteColumnNames(bool b);
//
// 	bool doNotWriteNULL () const;
// 	void doNotWriteNULL (bool b);
//
// void fieldDelimiter(const std::string& d);
//
//     const std::string& outputFile () const;
//     void outputFile (const std::string& fn);
//
//     const std::string& outputFormat () const;
//     void outputFormat (const std::string& s);
//
//     bool displayBitfieldsBinary () const;
//     void displayBitfieldsBinary (bool b);
//
//     bool displayBitfieldsHexadecimal () const;
//     void displayBitfieldsHexadecimal (bool b);
//
//     bool disableAlignmentOfColumns () const;
//     void disableAlignmentOfColumns (bool b);
//
//     bool fullPrecision () const;
//     void fullPrecision (bool);
//
// 	static const SQLOutputConfig defaultConfig();
//
// private:
// 	bool doNotWriteColumnNames_;
// 	bool doNotWriteNULL_;
//     std::string fieldDelimiter_;
//     std::string outputFile_;          // -o
//     std::string outputFormat_;        // -f
//     bool displayBitfieldsBinary_;     // --binary
//     bool displayBitfieldsHexadecimal_; // --hex
//     bool disableAlignmentOfColumns_;  // --no_alignment
//     bool fullPrecision_;              // --full_precision
//    std::string outputFormat_;        // -f
//    bool displayBitfieldsHexadecimal_; // --hex
//
// };

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
