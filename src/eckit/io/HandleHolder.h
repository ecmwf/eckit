/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - ECMWF Dec 2013

#pragma once
#include "eckit/io/DataHandle.h"


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
