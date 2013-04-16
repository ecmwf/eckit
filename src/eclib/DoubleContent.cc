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
#include "eckit/parser/Translator.h"
#include "eclib/NumberContent.h"
#include "eckit/parser/JSON.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ClassSpec DoubleContent::classSpec_ = {&Content::classSpec(),"DoubleContent",};
Reanimator<DoubleContent> DoubleContent::reanimator_;

DoubleContent::DoubleContent(double l):
    value_(l)
{
}

DoubleContent::DoubleContent(Stream& s):
    Content(s)
{
    s >> value_;
}

void DoubleContent::encode(Stream& s) const
{
    Content::encode(s);
    s << value_;
}

DoubleContent::~DoubleContent()
{
}

void DoubleContent::print(ostream& s) const
{
    s << value_;
}

void DoubleContent::json(JSON& s) const
{
    s << value_;
}


int DoubleContent::compare(const Content& other) const
{
    return -other.compareDouble(*this);
}

int DoubleContent::compareDouble(const DoubleContent& other) const
{
    double dif = (value_ - other.value_);
    if(dif == 0)
        return dif;
    if(dif<0)
        return -1;
    return 1;
}

int DoubleContent::compareNumber(const NumberContent& other) const
{
    double dif = (value_ - other.value_);
    if(dif == 0)
        return dif;
    if(dif<0)
        return -1;
    return 1;
}

void DoubleContent::value(double& l) const
{ 
    l = value_;
}

void DoubleContent::value(string& s) const
{ 
    s = Translator<double,string>()(value_);
}

Content* DoubleContent::add(const Content& other) const
{
    return other.addDouble(*this);
}

Content* DoubleContent::addDouble(const DoubleContent& other) const
{
    return new DoubleContent(other.value_ + value_);
}

Content* DoubleContent::sub(const Content& other) const
{
    return other.subDouble(*this);
}

Content* DoubleContent::subDouble(const DoubleContent& other) const
{
    return new DoubleContent(other.value_ - value_);
}

Content* DoubleContent::mul(const Content& other) const
{
    return other.mulDouble(*this);
}

Content* DoubleContent::mulDouble(const DoubleContent& other) const
{
    return new DoubleContent(other.value_ * value_);
}

Content* DoubleContent::div(const Content& other) const
{
    return other.divDouble(*this);
}

Content* DoubleContent::mod(const Content& other) const
{
    return other.modDouble(*this);
}


Content* DoubleContent::divDouble(const DoubleContent& other) const
{
    return new DoubleContent(other.value_ / value_);
}

Value DoubleContent::negate() const
{
    return Value(-value_);
}

//-----------------------------------------------------------------------------

} // namespace eckit

