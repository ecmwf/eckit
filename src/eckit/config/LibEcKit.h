// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#pragma once

#include "eckit/system/Library.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

using abort_handler_t = void (*)();

class LibEcKit : public eckit::system::Library {
public:

    LibEcKit();

    static LibEcKit& instance();
    void setAbortHandler(abort_handler_t h);
    void abort();
    bool dontDeregisterFactories() const;

    virtual std::string version() const;
    virtual std::string gitsha1(unsigned int count) const;

protected:

    const void* addr() const;

private:

    abort_handler_t abort_handler_;
    bool dontDeregisterFactories_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
