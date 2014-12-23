/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - (c) ECMWF Feb 12

#ifndef eckit_FixedString_h
#define eckit_FixedString_h

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// To be used as a key or value in BTree or other file-based classed

template< int size >
class FixedString {
public:

    static const size_t SIZE = size;

    FixedString();
    FixedString(const std::string&);
    FixedString(const FixedString&);
    FixedString(const char*);

    FixedString& operator=(const FixedString& other);

    bool operator<(const FixedString& other) const {
        return memcmp(data_, other.data_, size) < 0;
    }

    bool operator>(const FixedString& other) const {
        return memcmp(data_, other.data_, size) > 0;
    }

    bool operator==(const FixedString& other) const {
        return memcmp(data_, other.data_, size) == 0;
    }

    bool operator!=(const FixedString& other) const {
        return memcmp(data_, other.data_, size) != 0;
    }

    bool operator>=(const FixedString& other) const {
        return memcmp(data_, other.data_, size) >= 0;
    }

    bool operator<=(const FixedString& other) const {
        return memcmp(data_, other.data_, size) <= 0;
    }

    size_t length() const;

    std::string asString() const;
    
    operator std::string() const;

	char* data() { return data_; }
	const char* data() const { return data_; }

    static size_t static_size() { return SIZE; }

private:

    char data_[size];

    void print(std::ostream& s) const ;

    friend std::ostream& operator<<(std::ostream& s,const FixedString& p)
    {
        p.print(s);
        return s;
    }
};


//-----------------------------------------------------------------------------

} // namespace eckit

#include "FixedString.cc"

#endif
