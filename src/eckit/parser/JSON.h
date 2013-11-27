/*
 * (C) Copyright 1996-2013 ECMWF.
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
	
	JSON& operator<<(const string&);
	JSON& operator<<(const char*);

    JSON& null();

    JSON& startObject();
    JSON& endObject();

    JSON& startList();
    JSON& endList();

private: // members

    std::ostream& out_;
    vector<string> sep_;
    vector<bool> state_;
    bool null_;

private: // methods

    void sep();
    bool inlist() { return !state_.back(); }
    bool indict() { return  state_.back(); }

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
