/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/value/AnyContent.h"
#include "eckit/parser/JSON.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


ClassSpec AnyContent::classSpec_ = {&Content::classSpec(),"AnyContent",};
Reanimator<AnyContent> AnyContent::reanimator_;


AnyContent::AnyContent(const string& s): 
	value_(s) 
{ 
}

AnyContent::AnyContent(const char* s): 
	value_(s)
{ 
}

AnyContent::AnyContent(Stream& s):
	Content(s)
{
	s >> value_;
}

void AnyContent::encode(Stream& s) const
{
	Content::encode(s);
	s << value_;
}

AnyContent::~AnyContent()
{
}

void AnyContent::print(ostream& s) const
{
	s << value_;
}

void AnyContent::json(JSON& s) const
{
    s << value_;
}

int AnyContent::compare(const Content& other) const
{
	return -other.compareString(*this);
}

int AnyContent::compareString(const AnyContent& other) const
{
	return ::strcmp(value_.c_str(),other.value_.c_str());
}

void AnyContent::value(string& s) const 
{ 
	s = value_; 
}

void AnyContent::value(bool& b) const 
{ 
    if( value_ == "true" || value_ == "on" || value_ == "yes" || value_ == "1" )
    {
        b = true;
    }
    else
    {
            if( value_ == "false" || value_ == "off" || value_ == "no" || value_ == "0" )
                b = false;
            else
                Content::value(b);
    }
}

Content* AnyContent::add(const Content& other) const
{
	return other.addString(*this);
}

Content* AnyContent::addString(const AnyContent& other) const
{
	return new AnyContent(other.value_ + value_);
}

Content* AnyContent::sub(const Content& other) const
{
    return other.subString(*this);
}

Content* AnyContent::mul(const Content& other) const
{
    return other.mulString(*this);
}

Content* AnyContent::div(const Content& other) const
{
    return other.divString(*this);
}

Content* AnyContent::mod(const Content& other) const
{
    return other.modString(*this);
}

//-----------------------------------------------------------------------------

} // namespace eckit

