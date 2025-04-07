/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   Dec 2018

#ifndef eckit_TmpDir_h
#define eckit_TmpDir_h

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TmpDir : public PathName, private NonCopyable {
public:

    TmpDir(const char* base = nullptr);

    TmpDir(TmpDir&& rhs);

    ~TmpDir() noexcept(false);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
