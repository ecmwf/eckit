// File NetAddress.h
// T. Quintino - ECMWF Feb 2012

#ifndef NetAddress_H
#define NetAddress_H

#include "eclib/Stream.h"


// The class NetAddress represents a host plus its connection port

class NetAddress {
public:

    friend Stream& operator<<(Stream&,const NetAddress&);
    friend Stream& operator>>(Stream&,NetAddress&);
	
    friend ostream& operator<<(ostream& s,const NetAddress& p)
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

    void print(ostream&) const;
};

#endif
