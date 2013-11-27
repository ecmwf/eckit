/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File NetAddress.h
// T. Quintino - ECMWF Feb 2012

#ifndef eckit_NetAddress_h
#define eckit_NetAddress_h

#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// The class NetAddress represents a host plus its connection port

class NetAddress {
public:

    friend Stream& operator<<(Stream&,const NetAddress&);
    friend Stream& operator>>(Stream&,NetAddress&);
	
    friend std::ostream& operator<<(std::ostream& s,const NetAddress& p)
        { p.print(s); return s; }

// Contructors

    NetAddress(const string&); // parses the string formated as hostname:port
    NetAddress(const string&, const int);
    NetAddress(const Stream&);

    NetAddress(const NetAddress&);

// Destructor

    ~NetAddress();

// Assignment

    NetAddress& operator=(const NetAddress&);

// Convertors

    /// @returns string formated as hostname:port
//    string asString() const;
    /// @returns string formated as hostname:port
//    operator string() const { return asString(); }

// Operators

// Methods

    string host() const { return host_; }
    int    port() const { return port_; }

private:

// Members

    string               host_;
    int                  port_;

// Methods

    void print(std::ostream&) const;
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
