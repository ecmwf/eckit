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
/// @date   August 2016

#pragma once

#include "eckit/system/Library.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

typedef void (*abort_handler_t)();

class LibEcKit : public eckit::system::Library {

public:  // methods

    LibEcKit();

    static LibEcKit& instance();

    void setAbortHandler(abort_handler_t h);

    void abort();

    bool dontDeregisterFactories() const;

protected:  // methods

    const void* addr() const;

    virtual std::string version() const;

    virtual std::string gitsha1(unsigned int count) const;

private:  // members

    abort_handler_t abort_handler_;
    bool dontDeregisterFactories_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
