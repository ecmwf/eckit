/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Loader.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_Loader_h
#define eckit_Loader_h

#include "eckit/eckit.h"

#include "eckit/container/ClassExtent.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Loader : public ClassExtent<Loader> {

public:  // methods

    Loader();
    virtual ~Loader();

    virtual void execute() = 0;

private:  // methods
          // There is no private copy constructor as this will confuse g++ 4.x.x
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
