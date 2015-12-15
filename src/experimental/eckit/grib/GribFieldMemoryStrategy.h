/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file GribFieldMemoryStrategy.h
/// @author Baudouin Raoult
/// @date November 2013

#ifndef GribFieldMemoryStrategy_H
#define GribFieldMemoryStrategy_H

#include "eckit/eckit.h"

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

class GribField;

class GribFieldMemoryStrategy {

public: // methods

    GribFieldMemoryStrategy();

    virtual ~GribFieldMemoryStrategy();

    void touch(GribField&);
    void purgeable(GribField&);
    void remove(GribField&);

    void newValues(GribField&);
    void newHandle(GribField&);

protected: // methods

    void print(std::ostream&) const; // Change to virtual if base class

private: // members

    std::set<GribField*> released_;
    size_t max_;
    size_t count_;

    friend std::ostream& operator<<(std::ostream& s,const GribFieldMemoryStrategy& p)  { p.print(s); return s; }

};

//------------------------------------------------------------------------------------------------------

}
} // namespace eckit

#endif
