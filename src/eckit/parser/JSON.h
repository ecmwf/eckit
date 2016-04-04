/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File JSON.h
// Baudouin Raoult - (c) ECMWF Aug 11

#ifndef eckit_JSON_h
#define eckit_JSON_h

#include "eckit/eckit.h"

#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class JSON : private NonCopyable {

public: // methods

    /// Contructor
	JSON(std::ostream&);

    /// Destructor
	~JSON();

	JSON& operator<<(bool);
	JSON& operator<<(char);
	JSON& operator<<(unsigned char);
	JSON& operator<<(int);
	JSON& operator<<(unsigned int);
	JSON& operator<<(long);
	JSON& operator<<(unsigned long);
	JSON& operator<<(long long);
	JSON& operator<<(unsigned long long);
	JSON& operator<<(float);
	JSON& operator<<(double);

	JSON& operator<<(const std::string&);
	JSON& operator<<(const char*);

    template < typename T >
    JSON& operator<<(const std::vector<T>&);
    template < typename T >
    JSON& operator<<(const std::map<std::string, T>&);

    JSON& null();

    JSON& startObject();
    JSON& endObject();

    JSON& startList();
    JSON& endList();

    /// Set the precision for float and double (works like std::setprecision)
    JSON& precision(int);

private: // members

    std::ostream& out_;
    std::vector<std::string> sep_;
    std::vector<bool> state_;
    bool null_;

private: // methods

    void sep();
    bool inlist() { return !state_.back(); }
    bool indict() { return  state_.back(); }

};

//-----------------------------------------------------------------------------

template < typename T >
JSON& JSON::operator<<(const std::vector<T> &v)
{
    startList();
    for( size_t i = 0; i < v.size(); ++i ) {
        *this << v[i];
    }
    endList();
    return *this;
}

template < typename T >
JSON& JSON::operator<<(const std::map<std::string, T> &m)
{
    startObject();

    for( typename std::map<std::string, T>::const_iterator it = m.begin(); it != m.end(); ++it )
        *this << (*it).first << (*it).second;

    endObject();
    return *this;
}

} // namespace eckit

#endif
