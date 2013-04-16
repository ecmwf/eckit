/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/machine.h"
#include "eclib/FixedString.h"
#include "eckit/exception/Exceptions.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// For use with BTree or other persitant classes

template<int size>
FixedString<size>::FixedString()
{
    zero(data_);
}

template<int size>
FixedString<size>:: FixedString(const string& s) {
    ASSERT(s.length() <= size && sizeof(s[0]) == 1);
    zero(data_);
    std::copy(s.begin(), s.end(), data_);
}

template<int size>
FixedString<size>:: FixedString(const FixedString& other)
{
    memcpy(data_,other.data_,size);
}

template<int size>
FixedString<size>::FixedString(const char* s) {
    ASSERT(sizeof(char) == 1 && strlen(s) <= size);
    zero(data_);
    memcpy(data_, s, strlen(s));
}

template<int size>
FixedString<size>& FixedString<size>::operator=(const FixedString& other)
{
    if (this != &other) {
        memcpy(data_,other.data_,size);
    }
    return *this;
}

template<int size>
size_t FixedString<size>::length() const
{
    return std::find(data_, data_ + size, 0) - data_;
}

template<int size>
string FixedString<size>::asString() const
{
    return string(data_, data_ + length());
}

template<int size>
void FixedString<size>::print(ostream& s) const
{
    s.write(data_,length());
}

template<int size>
FixedString<size>::operator string() const
{
    return string(data_, data_ + length());
}

//-----------------------------------------------------------------------------

} // namespace eckit
