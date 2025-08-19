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
/// @date   June 2017

#ifndef eckit_filesystem_URIManager_h
#define eckit_filesystem_URIManager_h

#include "eckit/eckit.h"


#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class URI;
class DataHandle;

class URIManager {
public:

    virtual bool authority() { return false; }
    virtual bool query() { return false; }
    virtual bool fragment() { return false; }

    virtual bool exists(const URI&) = 0;

    virtual DataHandle* newWriteHandle(const URI&)                                      = 0;
    virtual DataHandle* newReadHandle(const URI&)                                       = 0;
    virtual DataHandle* newReadHandle(const URI&, const OffsetList&, const LengthList&) = 0;

    virtual std::string asString(const URI&) const;
    virtual eckit::PathName path(const URI&) const;

    static bool exists(const std::string&);
    static URIManager& lookUp(const std::string&);

protected:

    URIManager(const std::string&);

    virtual ~URIManager();

    virtual void print(std::ostream&) const;

private:

    std::string name_;

    friend std::ostream& operator<<(std::ostream& s, const URIManager& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
