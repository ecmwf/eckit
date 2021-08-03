/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/value/Content.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/os/BackTrace.h"
#include "eckit/value/Value.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class BadConversion : public Exception {
public:
    BadConversion(const std::string& w, const CodeLocation& loc);
};

class BadComparison : public Exception {
public:
    BadComparison(const std::string& w, const CodeLocation& loc);
};

class BadOperator : public Exception {
public:
    BadOperator(const std::string& w, const CodeLocation& loc);
};

BadConversion::BadConversion(const std::string& w, const CodeLocation& loc) :
    Exception(std::string("Bad Conversion: ") + w, loc) {
    //    std::cout << what() << std::endl;
    //    std::cout << BackTrace::dump() << std::endl;
}

BadComparison::BadComparison(const std::string& w, const CodeLocation& loc) :
    Exception(std::string("Bad Comparison: ") + w, loc) {
    //    std::cout << what() << std::endl;
    //    std::cout << BackTrace::dump() << std::endl;
}

BadOperator::BadOperator(const std::string& w, const CodeLocation& loc) :
    Exception(std::string("Bad operator: ") + w, loc) {
    //    std::cout << what() << std::endl;
    //    std::cout << BackTrace::dump() << std::endl;
}


//----------------------------------------------------------------------------------------------------------------------

ClassSpec Content::classSpec_ = {
    &Streamable::classSpec(),
    "Content",
};
Reanimator<Content> Content::reanimator_;

void Content::encode(Stream& s) const {
    Streamable::encode(s);
}

Content::Content(Stream& s) :
    Streamable(s) {}

Content::~Content() {}

void Content::badConversion(const std::string& to) const {
    std::ostringstream s;
    s << "Cannot convert " << *this << " (" << typeName() << ") to " << to;
    throw BadConversion(s.str(), Here());
}

void Content::badComparison(const std::string& to) const {
    std::ostringstream s;
    s << "Cannot compare " << *this << " (" << typeName() << ") with " << to;
    throw BadComparison(s.str(), Here());
}

void Content::badOperator(const std::string& op, const std::string& to) const {
    std::ostringstream s;
    s << *this << " (" << typeName() << ") " << op << " " << to;
    throw BadOperator(s.str(), Here());
}

Value Content::remove(const Value&) {
    std::ostringstream s;
    s << *this << " (" << typeName() << ") method 'remove' not implemented";
    throw BadOperator(s.str(), Here());
}

void Content::append(const Value&) {
    std::ostringstream s;
    s << *this << " (" << typeName() << ") method 'append' not implemented";
    throw BadOperator(s.str(), Here());
}

Value& Content::element(const Value&) {
    std::ostringstream s;
    s << *this << " (" << typeName() << ") method 'element' not implemented";
    throw BadOperator(s.str(), Here());
}

Value Content::keys() const {
    std::ostringstream s;
    s << *this << " (" << typeName() << ") method 'keys' not implemented";
    throw BadOperator(s.str(), Here());
}

size_t Content::size() const {
    std::ostringstream s;
    s << *this << " (" << typeName() << ") method 'size' not implemented";
    throw BadOperator(s.str(), Here());
}


bool Content::contains(const Value&) const {
    std::ostringstream s;
    s << *this << " (" << typeName() << ") method 'contains' not implemented";
    throw BadOperator(s.str(), Here());
}

Value Content::negate() const {
    std::ostringstream s;
    s << *this << " (" << typeName() << ") method 'negate' not implemented";
    throw BadOperator(s.str(), Here());
}

void Content::value(long long&) const {
    badConversion("long long");
}

void Content::value(bool&) const {
    badConversion("bool");
}

void Content::value(double&) const {
    badConversion("double");
}

void Content::value(std::string&) const {
    badConversion("std::string");
}

void Content::value(Date&) const {
    badConversion("Date");
}

void Content::value(Time&) const {
    badConversion("Time");
}

void Content::value(DateTime&) const {
    badConversion("DateTime");
}

void Content::value(ValueMap&) const {
    badConversion("Map");
}


void Content::value(ValueList& v) const {
    // Cast away constness, so the Contnt can be attached by the value
    v.push_back(Value(const_cast<Content*>(this)));
}

bool Content::operator==(const Content& other) const {
    return (this->compare(other) == 0);
}

bool Content::operator<(const Content& other) const {
    return (this->compare(other) < 0);
}


int Content::compareNumber(const NumberContent&) const {
    badComparison("Number");
    return 0;
}

int Content::compareBool(const BoolContent&) const {
    badComparison("Bool");
    return 0;
}

int Content::compareDouble(const DoubleContent&) const {
    badComparison("Double");
    return 0;
}

int Content::compareString(const StringContent&) const {
    badComparison("String");
    return 0;
}

int Content::compareNil(const NilContent&) const {
    badComparison("Nil");
    return 0;
}

int Content::compareList(const ListContent&) const {
    badComparison("List");
    return 0;
}

int Content::compareMap(const MapContent&) const {
    badComparison("Map");
    return 0;
}

int Content::compareDate(const DateContent&) const {
    badComparison("Date");
    return 0;
}

int Content::compareTime(const TimeContent&) const {
    badComparison("Time");
    return 0;
}

int Content::compareDateTime(const DateTimeContent&) const {
    badComparison("DateTime");
    return 0;
}

int Content::compareOrderedMap(const OrderedMapContent&) const {
    badComparison("OrderedMap");
    return 0;
}

Content* Content::operator+(const Content& other) const {
    return add(other);
}

Content* Content::add(const Content& other) const {
    badOperator("+", other.typeName());
    return nullptr;
}

Content* Content::addNumber(const NumberContent&) const {
    badOperator("+", "Number");
    return nullptr;
}

Content* Content::addBool(const BoolContent&) const {
    badOperator("+", "Bool");
    return nullptr;
}

Content* Content::addDouble(const DoubleContent&) const {
    badOperator("+", "Number");
    return nullptr;
}

Content* Content::addString(const StringContent&) const {
    badOperator("+", "String");
    return nullptr;
}

Content* Content::addNil(const NilContent&) const {
    badOperator("+", "Nil");
    return nullptr;
}

Content* Content::addList(const ListContent&) const {
    badOperator("+", "List");
    return nullptr;
}

Content* Content::addMap(const MapContent&) const {
    badOperator("+", "List");
    return nullptr;
}

Content* Content::addDate(const DateContent&) const {
    badOperator("+", "Date");
    return nullptr;
}

Content* Content::addTime(const TimeContent&) const {
    badOperator("+", "Time");
    return nullptr;
}

Content* Content::addDateTime(const DateTimeContent&) const {
    badOperator("+", "DateTime");
    return nullptr;
}

Content* Content::addOrderedMap(const OrderedMapContent&) const {
    badOperator("+", "OrderedMap");
    return nullptr;
}

Content* Content::operator-(const Content& other) const {
    return sub(other);
}

Content* Content::sub(const Content& other) const {
    badOperator("-", other.typeName());
    return nullptr;
}

Content* Content::subNumber(const NumberContent&) const {
    badOperator("-", "Number");
    return nullptr;
}

Content* Content::subDouble(const DoubleContent&) const {
    badOperator("-", "Double");
    return nullptr;
}

Content* Content::subBool(const BoolContent&) const {
    badOperator("-", "Bool");
    return nullptr;
}

Content* Content::subString(const StringContent&) const {
    badOperator("-", "String");
    return nullptr;
}

Content* Content::subNil(const NilContent&) const {
    badOperator("-", "Nil");
    return nullptr;
}

Content* Content::subList(const ListContent&) const {
    badOperator("-", "List");
    return nullptr;
}

Content* Content::subMap(const MapContent&) const {
    badOperator("-", "Map");
    return nullptr;
}

Content* Content::subDate(const DateContent&) const {
    badOperator("-", "Date");
    return nullptr;
}

Content* Content::subTime(const TimeContent&) const {
    badOperator("-", "Time");
    return nullptr;
}

Content* Content::subDateTime(const DateTimeContent&) const {
    badOperator("-", "DateTime");
    return nullptr;
}

Content* Content::subOrderedMap(const OrderedMapContent&) const {
    badOperator("-", "OrderedMap");
    return nullptr;
}

Content* Content::operator*(const Content& other) const {
    return mul(other);
}

Content* Content::mul(const Content& other) const {
    badOperator("*", other.typeName());
    return nullptr;
}

Content* Content::mulNumber(const NumberContent&) const {
    badOperator("*", "Number");
    return nullptr;
}

Content* Content::mulBool(const BoolContent&) const {
    badOperator("*", "Bool");
    return nullptr;
}

Content* Content::mulDouble(const DoubleContent&) const {
    badOperator("*", "Double");
    return nullptr;
}

Content* Content::mulString(const StringContent&) const {
    badOperator("*", "String");
    return nullptr;
}

Content* Content::mulNil(const NilContent&) const {
    badOperator("*", "Nil");
    return nullptr;
}

Content* Content::mulList(const ListContent&) const {
    badOperator("*", "List");
    return nullptr;
}

Content* Content::mulMap(const MapContent&) const {
    badOperator("*", "Map");
    return nullptr;
}

Content* Content::mulDate(const DateContent&) const {
    badOperator("*", "Date");
    return nullptr;
}

Content* Content::mulTime(const TimeContent&) const {
    badOperator("*", "Time");
    return nullptr;
}

Content* Content::mulDateTime(const DateTimeContent&) const {
    badOperator("*", "DateTime");
    return nullptr;
}

Content* Content::mulOrderedMap(const OrderedMapContent&) const {
    badOperator("-", "OrderedMap");
    return nullptr;
}

Content* Content::operator/(const Content& other) const {
    return div(other);
}

Content* Content::div(const Content& other) const {
    badOperator("/", other.typeName());
    return nullptr;
}

Content* Content::divDouble(const DoubleContent&) const {
    badOperator("/", "Double");
    return nullptr;
}

Content* Content::divNumber(const NumberContent&) const {
    badOperator("/", "Number");
    return nullptr;
}

Content* Content::divBool(const BoolContent&) const {
    badOperator("/", "Bool");
    return nullptr;
}


Content* Content::divString(const StringContent&) const {
    badOperator("/", "String");
    return nullptr;
}

Content* Content::divNil(const NilContent&) const {
    badOperator("/", "Nil");
    return nullptr;
}

Content* Content::divList(const ListContent&) const {
    badOperator("/", "List");
    return nullptr;
}

Content* Content::divMap(const MapContent&) const {
    badOperator("/", "Map");
    return nullptr;
}

Content* Content::divDate(const DateContent&) const {
    badOperator("/", "Date");
    return nullptr;
}

Content* Content::divTime(const TimeContent&) const {
    badOperator("/", "Time");
    return nullptr;
}

Content* Content::divDateTime(const DateTimeContent&) const {
    badOperator("/", "DateTime");
    return nullptr;
}

Content* Content::divOrderedMap(const OrderedMapContent&) const {
    badOperator("/", "OrderedMap");
    return nullptr;
}

Content* Content::mod(const Content& other) const {
    badOperator("%", other.typeName());
    return nullptr;
}

Content* Content::modDouble(const DoubleContent&) const {
    badOperator("%", "Double");
    return nullptr;
}

Content* Content::modNumber(const NumberContent&) const {
    badOperator("%", "Number");
    return nullptr;
}

Content* Content::modBool(const BoolContent&) const {
    badOperator("%", "Bool");
    return nullptr;
}


Content* Content::modString(const StringContent&) const {
    badOperator("%", "String");
    return nullptr;
}

Content* Content::modNil(const NilContent&) const {
    badOperator("%", "Nil");
    return nullptr;
}

Content* Content::modList(const ListContent&) const {
    badOperator("%", "List");
    return nullptr;
}

Content* Content::modMap(const MapContent&) const {
    badOperator("%", "Map");
    return nullptr;
}

Content* Content::modDate(const DateContent&) const {
    badOperator("%", "Date");
    return nullptr;
}

Content* Content::modTime(const TimeContent&) const {
    badOperator("%", "Time");
    return nullptr;
}

Content* Content::modDateTime(const DateTimeContent&) const {
    badOperator("%", "DateTime");
    return nullptr;
}

Content* Content::modOrderedMap(const OrderedMapContent&) const {
    badOperator("%", "OrderedMap");
    return nullptr;
}

Content::Content() {}

#ifndef IBM
template <>
Streamable* Reanimator<Content>::ressucitate(Stream&) const {
    return nullptr;
}
#endif

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
