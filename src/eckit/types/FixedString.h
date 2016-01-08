/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Baudouin Raoult - (c) ECMWF Feb 12

#ifndef eckit_types_FixedString_h
#define eckit_types_FixedString_h

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// To be used as a key or value in BTree or other file-based classed

template< int SIZE >
class FixedString {
public:

    FixedString();
    FixedString(const std::string&);
    FixedString(const FixedString&);
    FixedString(const char*);

	FixedString& operator=(const FixedString&);
	FixedString& operator=(const std::string&);

	bool operator<(const FixedString& other) const { return memcmp(data_, other.data_, SIZE) < 0; }

	bool operator>(const FixedString& other) const { return memcmp(data_, other.data_, SIZE) > 0; }

	bool operator==(const FixedString& other) const { return memcmp(data_, other.data_, SIZE) == 0; }

	bool operator!=(const FixedString& other) const { return memcmp(data_, other.data_, SIZE) != 0; }

	bool operator>=(const FixedString& other) const { return memcmp(data_, other.data_, SIZE) >= 0; }

	bool operator<=(const FixedString& other) const { return memcmp(data_, other.data_, SIZE) <= 0; }

	size_t length() const;

    std::string asString() const;
    
    operator std::string() const;

	char* data() { return data_; }
	const char* data() const { return data_; }

	size_t size() { return SIZE; }

	static size_t static_size() { return SIZE; }

private:

	char data_[SIZE];

    void print(std::ostream& s) const ;

    friend std::ostream& operator<<(std::ostream& s,const FixedString& p)
    {
        p.print(s);
        return s;
    }
};

//-----------------------------------------------------------------------------

template<int SIZE>
FixedString<SIZE>::FixedString()
{
	zero(data_);
}

template<int SIZE>
FixedString<SIZE>:: FixedString(const std::string& s)
{
	ASSERT(s.length() <= SIZE && sizeof(s[0]) == 1);
	zero(data_);
	std::copy(s.begin(), s.end(), data_);
}

template<int SIZE>
FixedString<SIZE>:: FixedString(const FixedString& other)
{
	memcpy(data_,other.data_,SIZE);
}

template<int SIZE>
FixedString<SIZE>::FixedString(const char* s) {
	ASSERT(sizeof(char) == 1 && strlen(s) <= SIZE);
	zero(data_);
	memcpy(data_, s, strlen(s));
}

template<int SIZE>
FixedString<SIZE>& FixedString<SIZE>::operator=(const FixedString& s)
{
	if (this != &s)
	{
		memcpy(data_,s.data_,SIZE);
	}
	return *this;
}

template<int SIZE>
FixedString<SIZE>& FixedString<SIZE>::operator=(const std::string& s)
{
	ASSERT(s.length() <= SIZE && sizeof(s[0]) == 1);

	memcpy(data_,s.c_str(),s.length());

	return *this;
}

template<int SIZE>
size_t FixedString<SIZE>::length() const
{
	return std::find(data_, data_ + SIZE, 0) - data_;
}

template<int SIZE>
std::string FixedString<SIZE>::asString() const
{
	return std::string(data_, data_ + length());
}

template<int SIZE>
void FixedString<SIZE>::print(std::ostream& s) const
{
	s.write(data_,length());
}

template<int SIZE>
FixedString<SIZE>::operator std::string() const
{
	return std::string(data_, data_ + length());
}

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
