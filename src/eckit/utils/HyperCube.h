/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HyperCube.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef eckit_HyperCube_h
#define eckit_HyperCube_h

#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// Helper class to handle multi-dimension objects
// The first dimension should be the one most likely to change

class HyperCube {
public:


	typedef std::vector<Ordinal> Dimensions;
	typedef std::vector<Ordinal> Coordinates;
	typedef std::vector<Ordinal> Remapping;

// -- Contructors

	HyperCube(const Dimensions& d) : dimensions_(d) {}

// -- Methods

	// Translate coordinates into an index to a 1 dimension array.

	Ordinal index(const Coordinates&) const;

	// Return the number of elemets

	Ordinal count() const;

	// Translate index to coordinates

	void coordinates(Ordinal index, Coordinates&) const;

	// Accessors

	const Dimensions& dimensions() const    { return dimensions_;        }
	Dimensions& dimensions()                { return dimensions_;        }
	Ordinal     dimensions(Ordinal n) const { return dimensions_[n];     }
	Ordinal     size() const                { return dimensions_.size(); }

	// Return the 'remapping' std::vector needing to add 'count' labels
	// for the dimension 'which' at position 'where'

	HyperCube addToDimension(Ordinal which,Ordinal where,Ordinal count,Remapping&) const;

	// Combine two 'remapping' vectors

	static void combine(Remapping&,const Remapping&);

private:

// -- Members

	Dimensions dimensions_;

};

inline  // For speed
Ordinal HyperCube::count() const
{
    return accumulate(dimensions_.begin(),dimensions_.end(),1,std::multiplies<Ordinal>());
}


inline // For speed
Ordinal HyperCube::index(const Coordinates& coord) const
{
	Ordinal n = 1;
	Ordinal a = 0;

	ASSERT(coord.size() == dimensions_.size());

	// The fact that this is in reverse is important for addToDimension

	for(int i = coord.size()-1; i >= 0; i--)
	{
		ASSERT(/*coord[i] >= 0 &&*/ coord[i] < dimensions_[i]);
		a += coord[i] * n;
		n *= dimensions_[i];
	}

	return a;
}


inline // For speed
void HyperCube::combine(Remapping& map1,const Remapping& map2)
{
	if(map1.size() == 0)
		map1 = map2;
	else for(Remapping::iterator i = map1.begin() ; i != map1.end() ; ++i)
	{
		ASSERT(*i < map2.size());
		*i = map2[*i];
	}
}


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
