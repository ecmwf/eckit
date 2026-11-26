// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Sep 2001

#ifndef eckit_TmpFile_h
#define eckit_TmpFile_h

#include "eckit/filesystem/PathName.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TmpFile : public PathName {
public:

    TmpFile(bool verbose = true);

    TmpFile(const TmpFile&)            = delete;
    TmpFile& operator=(const TmpFile&) = delete;
    TmpFile(TmpFile&& rhs)             = delete;
    TmpFile& operator=(TmpFile&& rhs)  = delete;

    ~TmpFile();

private:

    bool verbose_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
