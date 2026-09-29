// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Simon Smart
/// @date   Dec 2018

#ifndef eckit_TmpDir_h
#define eckit_TmpDir_h

#include "eckit/filesystem/PathName.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TmpDir : public PathName {
public:

    TmpDir(const char* base = nullptr);

    TmpDir(const TmpDir&)            = delete;
    TmpDir& operator=(const TmpDir&) = delete;

    TmpDir(TmpDir&& rhs);
    TmpDir& operator=(TmpDir&& rhs) = delete;

    ~TmpDir() noexcept(false);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
