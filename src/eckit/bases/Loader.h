// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Loader.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_Loader_h
#define eckit_Loader_h

#include "eckit/container/ClassExtent.h"
#include "eckit/eckit.h"

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
