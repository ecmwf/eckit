/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File NetAddress.cc
// T. Quintino - ECMWF Feb 2012

#include "NetAddress.h"

#include "eckit/parser/Tokenizer.h"
#include "eckit/utils/Translator.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

Stream& operator<<(Stream& s,const NetAddress& addr)
{
    s << addr.host_ << addr.port_;
    return s;
}

Stream& operator>>(Stream& s,NetAddress& addr)
{
    s >> addr.host_ >> addr.port_;
    return s;
}


NetAddress::NetAddress(const std::string& host, const int port) :
    host_(host),
    port_(port)
{
}

NetAddress::NetAddress(const std::string& s)
{
    Tokenizer tokenize(":");
    std::vector<std::string> tokens;
    tokenize(s,tokens);
    ASSERT(tokens.size() == 2);
    host_ = tokens[0];
    port_ = Translator<std::string,int>()( tokens[1] );

    // check that port_ > 0
}

NetAddress::NetAddress(const NetAddress& other) :
    host_(other.host_),
    port_(other.port_)
{
}

NetAddress::~NetAddress()
{
}

NetAddress& NetAddress::operator=(const NetAddress& other)
{
	if(this != &other) {
        host_ = other.host_;
        port_ = other.port_;
    }
	return *this;
}

#if 0
std::string NetAddress::asString() const
{
    stringstream os;
    os << *this;
    return os.std::string();
}
#endif

void NetAddress::print(std::ostream& s) const
{
    s << host_ << ":" << port_;
}

//-----------------------------------------------------------------------------

} // namespace eckit

