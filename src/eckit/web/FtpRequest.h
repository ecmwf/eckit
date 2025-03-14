/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FtpRequest.h
// Baudouin Raoult - ECMWF Feb 02

#ifndef FtpRequest_H
#define FtpRequest_H

#include <string>

#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

class Retrieve;

namespace eckit {

class Length;
class PathName;

class FtpRequest : private eckit::NonCopyable {
public:

    // -- Contructors

    FtpRequest() {}

    // -- Destructor

    virtual ~FtpRequest() {}

    // -- Methods

    virtual std::string next() = 0;

    virtual bool here()   = 0;
    virtual void cdDown() = 0;
    virtual void cdUp()   = 0;

    virtual void directory(const std::string&)                             = 0;
    virtual void file(const std::string&, const eckit::Length&, Retrieve*) = 0;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
