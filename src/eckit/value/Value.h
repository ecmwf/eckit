/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Value.h
/// @author Manuel Fuentes
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Jun 97

#ifndef eckit_Value_h
#define eckit_Value_h

#include "eckit/value/Content.h"
#include "eckit/types/Date.h"
#include "eckit/types/DateTime.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Length;
class PathName;
class JSON;

class Value {
public:

// -- Contructors

    Value();

    Value(bool);
    Value(int);
    Value(long);
    Value(long long);
    Value(unsigned int);
    Value(unsigned long);
    Value(unsigned long long);
    Value(double);

    Value(const std::string&);
    Value(const char*);

    Value(const Length&);
    Value(const Date&);
    Value(const Time&);
    Value(const DateTime&);
    Value(const PathName&);

    Value(Stream&);
    Value(const ValueList&);
    Value(const ValueMap&);

// -- Copy

	Value(const Value&);
	Value& operator=(const Value&);

// -- Destructor

	~Value();

// -- Operators

	template< typename T > T as() { T r; content_->value(r); return r; }

    operator short() const              { long long l;  content_->value(l); return l; }
    operator unsigned short() const     { long long l;  content_->value(l); return l; }

    operator int() const                { long long l;  content_->value(l); return l; }
    operator unsigned int() const       { long long l;  content_->value(l); return l; }

    operator long() const               { long long l;  content_->value(l); return l; }
    operator unsigned long() const      { long long l;  content_->value(l); return l; }

    operator long long() const          { long long l;  content_->value(l); return l; }
    operator unsigned long long() const { long long l;  content_->value(l); return l; }

    operator double() const             { double d;     content_->value(d); return d; }
    operator bool() const               { bool d;       content_->value(d); return d; }

    operator std::string() const  { std::string s; content_->value(s); return s; }
    operator PathName() const     { std::string s; content_->value(s); return s; }

    operator Date() const      { Date d; content_->value(d); return d; }
    operator Time() const      { Time t; content_->value(t); return t; }
    operator DateTime() const  { DateTime d; content_->value(d); return d; }

    operator Length() const    { long long l;  content_->value(l); return l; }
    operator Offset() const    { long long l;  content_->value(l); return l; }

    operator ValueList() const;
    operator ValueMap() const;

    bool operator<(const Value& v) const  { return *content_ < *(v.content_); }
    bool operator==(const Value& v) const { return *content_ == *(v.content_); }

    bool operator>(const Value& v) const  { return v < *this;     }
    bool operator!=(const Value& v) const { return !(*this == v); }

    bool operator>=(const Value& v) const  { return !(*this < v);     }
    bool operator<=(const Value& v) const  { return !(v < *this);     }


    Value operator+(const Value&) const;
    Value& operator+=(const Value&);

    Value operator-() const;

    Value operator-(const Value&) const;
    Value& operator-=(const Value&);

    Value operator*(const Value&) const;
    Value& operator*=(const Value&);

    Value operator/(const Value&) const;
    Value& operator/=(const Value&);

    Value operator%(const Value&) const;
    Value& operator%=(const Value&);

    Value operator[](const char*) const;
    Value operator[](const std::string&) const;
    Value operator[](const Value&) const;
    Value operator[](int) const;

    bool contains(const char*) const;
    bool contains(const std::string&) const;
    bool contains(const Value&) const;
    bool contains(int) const;

    // -- Methods

    int      compare(const Value& v) const { return content_->compare(*(v.content_)); }

    bool     isNil()      const { return content_->isNil(); }
    bool     isNumber()   const { return content_->isNumber(); }
    bool     isBool()     const { return content_->isBool(); }
    bool     isDouble()   const { return content_->isDouble(); }
    bool     isString()   const { return content_->isString(); }
    bool     isList()     const { return content_->isList(); }
    bool     isMap()      const { return content_->isMap(); }
    bool     isDate()     const { return content_->isDate(); }
    bool     isTime()     const { return content_->isTime(); }
    bool     isDateTime() const { return content_->isDateTime(); }

    Value	 tail() const;
    Value	 head() const;

    Value    clone() const;
    bool     shared() const; // Ensure that value is not shared

// -- Class Methods

    static Value makeList();
    static Value makeList(const Value&);
    static Value makeList(const ValueList&);

    static Value makeMap();
    static Value makeMap(const ValueMap&);

protected:

	Value(Content*);


private:

// -- Members

	Content* content_;

// -- Methods

    void json(JSON& s) const        { s << *content_; }
    void print(std::ostream& s) const    { s << *content_; }
    void encode(Stream& s) const    { s << *content_; }

    friend JSON& operator<<(JSON& s, const Value& v) { v.json(s);  return s; }
	friend std::ostream& operator<<(std::ostream& s, const Value& v) { v.print(s);  return s; }
	friend Stream&  operator<<(Stream&  s, const Value& v) { v.encode(s); return s; }

	friend class Content;

};

//-----------------------------------------------------------------------------

template < typename T >
Value makeVectorValue( const std::vector<T>& v )
{
	ValueList r;
	r.reserve(v.size());
	for( size_t i = 0; i < v.size(); ++i )
		r.push_back( Value(v[i]) );
	return Value::makeList(r);
}

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
