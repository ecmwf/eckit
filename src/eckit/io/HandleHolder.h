// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Baudouin Raoult - ECMWF Dec 2013

#ifndef eckit_filesystem_HandleHolder_h
#define eckit_filesystem_HandleHolder_h

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/log/Timer.h"
#include "eckit/types/Types.h"


namespace eckit {

//-----------------------------------------------------------------------------

class HandleHolder {
public:

    // -- Contructors

    HandleHolder(DataHandle& handle);
    HandleHolder(DataHandle* handle);

    // -- Destructor

    ~HandleHolder();

    // -- Methods

    // -- Overridden methods


protected:

    DataHandle& handle() { return *handle_; }
    const DataHandle& handle() const { return *handle_; }

private:

    // -- Members

    DataHandle* handle_;
    bool owned_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
