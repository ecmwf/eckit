/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/DoubleContent.h"
#include "eclib/Exceptions.h"
#include "eclib/JSON.h"
#include "eclib/NumberContent.h"
#include "eclib/StrStream.h"
#include "eclib/Translator.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------


class BadBoolConversion:  public Exception { 
	public: 
		BadBoolConversion(const string& w):
			Exception(string("Bad Bool Conversion: ") + w)   {  }
};

//=============================================================================

ClassSpec NumberContent::classSpec_ = {&Content::classSpec(),"NumberContent",};
Reanimator<NumberContent> NumberContent::reanimator_;

NumberContent::NumberContent(long long l): 
    value_(l)
{
}

NumberContent::NumberContent(Stream& s):
	Content(s)
{
    s >> value_;
}

void NumberContent::encode(Stream& s) const
{
	Content::encode(s);
    s << value_;
}

NumberContent::~NumberContent()
{
}

void NumberContent::print(ostream& s) const
{
    s << value_;
}

void NumberContent::json(JSON& s) const
{
    s << value_;
}

int NumberContent::compare(const Content& other) const
{
	return -other.compareNumber(*this);
}

int NumberContent::compareNumber(const NumberContent& other) const
{
    long long dif = (value_ - other.value_);
	if(dif == 0)
		return dif;
	if(dif<0)
		return -1;

	return 1;
}

int NumberContent::compareDouble(const DoubleContent& other) const
{
    double dif = (value_ - other.value_);
    if(dif == 0)
        return dif;
    if(dif<0)
        return -1;
    return 1;
}

void NumberContent::value(bool& b) const 
{ 
    if( value_ == 0 )
        b = false;
    else
        b = true;
}

void NumberContent::value(long long& l) const 
{ 
    l = value_;
}

void NumberContent::value(double& l) const
{
    l = value_;
}

void NumberContent::value(string& s) const 
{ 
    s = Translator<long long,string>()(value_);
}

Content* NumberContent::add(const Content& other) const
{
	return other.addNumber(*this);
}

Content* NumberContent::addNumber(const NumberContent& other) const
{
    return new NumberContent(other.value_ + value_);
}

Content* NumberContent::sub(const Content& other) const
{
	return other.subNumber(*this);
}

Content* NumberContent::subNumber(const NumberContent& other) const
{
    return new NumberContent(other.value_ - value_);
}

Content* NumberContent::mul(const Content& other) const
{
	return other.mulNumber(*this);
}

Content* NumberContent::mod(const Content& other) const
{
    return other.modNumber(*this);
}

Content* NumberContent::mulNumber(const NumberContent& other) const
{
    return new NumberContent(other.value_ * value_);
}

Content* NumberContent::div(const Content& other) const
{
	return other.divNumber(*this);
}

Content* NumberContent::divNumber(const NumberContent& other) const
{
    return new NumberContent(other.value_ / value_);
}

Value NumberContent::negate() const
{
    return Value(-value_);
}

//-----------------------------------------------------------------------------

} // namespace eclib

