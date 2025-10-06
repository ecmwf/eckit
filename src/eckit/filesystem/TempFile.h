/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date   Nov 1996

#ifndef eckit_TempFile_h
#define eckit_TempFile_h

#include "eckit/filesystem/PathName.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TempFile : public PathName {
public:

    TempFile();
    TempFile(const std::string&); /* the std::string is the name of a filespace */

    ~TempFile();

private:

    static PathName dir(const std::string&);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
