/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Plural_h
#define eckit_Plural_h

#include "eckit/log/BigNum.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Plural {
public:

// -- Contructors

	Plural(int count,const std::string& s) : s_(s),count_(count) {}

// -- Destructor

	~Plural() {}

protected:

// -- Methods

	void print(std::ostream& s) const
	{
		s << BigNum(count_) << ' ' << s_;
		if(count_>1) s << 's';
	}

private:

// There is no private copy constructor as this will confuse g++ 4.x.x

	std::string s_;
	int count_;

// -- Friends

	friend std::ostream& operator<<(std::ostream& s,const Plural& p)
		{ p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
