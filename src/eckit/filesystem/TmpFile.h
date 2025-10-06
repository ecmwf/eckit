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
/// @author Tiago Quintino
/// @date   Sep 2001

#ifndef eckit_TmpFile_h
#define eckit_TmpFile_h

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TmpFile : public PathName, private NonCopyable {
public:

    TmpFile(bool verbose = true);

    ~TmpFile();

private:

    bool verbose_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
