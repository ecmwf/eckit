// File NetAddress.cc
// T. Quintino - ECMWF Feb 2012

#include "NetAddress.h"

#include "eclib/Tokenizer.h"
#include "eclib/Translator.h"

//-----------------------------------------------------------------------------

namespace eclib {

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


NetAddress::NetAddress(const string& host, const int port) :
    host_(host),
    port_(port)
{
}

NetAddress::NetAddress(const string& s)
{
    Tokenizer tokenize(":");
    vector<string> tokens;
    tokenize(s,tokens);
    ASSERT(tokens.size() == 2);
    host_ = tokens[0];
    port_ = Translator<string,int>()( tokens[1] );

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
string NetAddress::asString() const
{
    stringstream os;
    os << *this;
    return os.string();
}
#endif

void NetAddress::print(ostream& s) const
{
    s << host_ << ":" << port_;
}

//-----------------------------------------------------------------------------

} // namespace eclib

