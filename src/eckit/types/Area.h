/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Area.h
// Claude Gibert - ECMWF Dec 97

#ifndef eckit_Area_h
#define eckit_Area_h

#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Area {
public:

	friend ostream& operator<<(ostream& s,const Area& x)
		{ x.print(s); return s; }

	friend Stream& operator>>(Stream& s,Area& x)
		{ return s >> x.top_ >> x.left_ >> x.bottom_ >> x.right_; }

	void print(ostream& s) const;
	
// -- Contructors

	Area() : top_(0),left_(0),bottom_(0),right_(0) {}
	
	Area(const Latitude& t,
		const Longitude& l,
		const Latitude& b,
		const Longitude& r);

	Area(const Point& topLeft,
		const Point& botRight);

	// Copy 
	Area(const Area& other);

	~Area();


// Methods

	Point	topLeft()	const 	{ return Point(top_,left_); }
	Point	botRight()	const	{ return Point(bottom_,restore(right_)); }


	Area& unionWith(const Area& other);
	Area& sectWith(const Area& other);
	bool contains(const Area& other) const
			{ return contains(*this,other); }
	
	void set(const Latitude& t,
			const Longitude& l,
			const Latitude& b,
			const Longitude& r);
	void offset(double dh,double dv);

	bool pointInArea(const Point&) const;
	bool pointInArea(const Latitude& lat,const Longitude& longi) const;
	bool isEmpty() const { return top_ >= bottom_ && right_ <= left_; }

	static Area doUnion(const Area& one,const Area& two);
	static Area doSect(const Area& one,const Area& two);
	static bool intersect(const Area& one,const Area& two);
	static bool contains(const Area& container,const Area& contained);
	static Area emptyArea() { return Area(0,0,0,0); }

// -- Operators

	Area& operator=(const Area& other);

private:
// -- Methods
	void validate();
	Longitude restore(const Longitude&) const;


// -- Members

	Latitude	top_;
	Longitude	left_;
	Latitude	bottom_;
	Longitude	right_;

};

typedef vector<Area> AreaList;

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
