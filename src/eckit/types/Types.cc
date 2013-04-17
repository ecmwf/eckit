/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/machine.h"

#include "eckit/types/Types.h"
#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template<class T>
void operator<<(Stream& s,const vector<T>& t)
{
	s << Ordinal(t.size());
	for(typename vector<T>::const_iterator i = t.begin(); i != t.end() ; ++i)
		s << (*i);
}

template<class T>
void operator>>(Stream& s,vector<T>& t)
{

	Ordinal size;
	s >> size;

	t.clear();
	t.reserve(size);

	for(Ordinal i = 0; i < size; i++)
	{
		T n;
		s >> n;
		t.push_back(n);
	}
}


//====================================================================

template<class T>
output_list<T>::output_list(ostream& s):
    first_(true), s_(s)
{
	s_ << '[';
}

template<class T>
output_list<T>::~output_list()
{
	flush();
	s_ << ']';
}

template<class T>
void output_list<T>::push_back(const T& value)
{
	if(v_.size() < 2 )
		v_.push_back(value);
	else {
		long long diff1 = v_[1]  - v_[0];
		long long diff2 = value  - v_.back();

		if(diff1 != diff2 || diff2 < 0 )
			flush();

		v_.push_back(value);
	}
}

template<class T>
void output_list<T>::flush()
{
	long long diff;

	if(!first_) s_ << ',';

	switch(v_.size())
	{
		case 0: break;
		case 1: s_ << v_[0] ; break;
		case 2: s_ << v_[0] << ',' << v_[1] ; break;
		default:
			diff = v_[1]  - v_[0];
#ifdef __hpux
			switch(int(diff))
#else
			switch(diff)
#endif
			{
				case 0:
					s_ << v_.size() << '*' << v_[0];
					break;

				case 1:
					s_ << v_[0] << '-' << v_.back();
					break;

				default:
					s_ << v_[0] << '-' << v_.back() << '-' << diff;
					break;
			}
			break;

	}
	v_.clear();
	first_ = false;
}

//-----------------------------------------------------------------------------

} // namespace eckit

