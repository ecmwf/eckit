/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - (c) ECMWF Feb 12

#ifndef FixedString_H
#define FixedString_H

#include "eclib/machine.h"

// To be used as a key or value in BTree or other file-based classed

template<int size>
class FixedString {
public:

    FixedString();
    FixedString(const string&);
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
    
    operator string() const;

private:

    char data_[size];

    void print(ostream& s) const ;

    friend ostream& operator<<(ostream& s,const FixedString& p)
    {
        p.print(s);
        return s;
    }
};

#include "FixedString.cc"
#endif
