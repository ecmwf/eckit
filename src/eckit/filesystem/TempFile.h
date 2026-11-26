// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
