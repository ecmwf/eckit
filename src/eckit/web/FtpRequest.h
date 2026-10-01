// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FtpRequest.h
// Baudouin Raoult - ECMWF Feb 02

#ifndef FtpRequest_H
#define FtpRequest_H

#include <string>

//-----------------------------------------------------------------------------

class Retrieve;

namespace eckit {

class Length;
class PathName;

class FtpRequest {
public:

    // -- Contructors

    FtpRequest() = default;

    FtpRequest(const FtpRequest&)            = delete;
    FtpRequest& operator=(const FtpRequest&) = delete;
    FtpRequest(FtpRequest&&)                 = delete;
    FtpRequest& operator=(FtpRequest&&)      = delete;

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
