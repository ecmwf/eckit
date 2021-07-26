/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/serialisation/Streamable.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {


class NotSubClass : public Exception {
public:
    NotSubClass(const std::string&, const std::string&);
};

class UnknowClass : public Exception {
public:
    UnknowClass(const std::string&);
};


//----------------------------------------------------------------------------------------------------------------------

// Should protected with a mutex...

typedef std::map<std::string, ReanimatorBase*, std::less<std::string> > Map;

// This trick garanty than a std::map is created

static Map& theMap() {
    static Map m;
    return m;
}

ReanimatorBase::ReanimatorBase(const ClassSpec& spec) :
    spec_(spec) {
    theMap()[std::string(spec_.name_)] = this;
    //	std::cout << "ReanimatorBase::ReanimatorBase " << spec_.name_ << std::endl;
}

ReanimatorBase::~ReanimatorBase() {
    // Should not be there
    // remove ReanimatorBase form list
}


UnknowClass::UnknowClass(const std::string& w) :
    Exception(std::string("Unknow class: ") + w) {}

NotSubClass::NotSubClass(const std::string& found, const std::string& clss) :
    Exception(std::string("Not a sub class: object ") + found + std::string(" found, but it is not subclass of ") + clss) {}


Streamable* ReanimatorBase::reanimate(Stream& s, const ClassSpec* c) {
    if (!s.next())
        return 0;

    std::string name;

    s >> name;

    Map::iterator i = theMap().find(name);
    if (i == theMap().end())
        throw UnknowClass(name);

    ReanimatorBase* r = (*i).second;

    // Check for the class

    if (c) {
        const ClassSpec* a = &r->spec_;
        while (a != 0 && a != c)
            a = a->superClass_;

        if (a == 0)
            throw NotSubClass(name, c->name_);
    }

    Streamable* x = r->ressucitate(s);
    s.skipEndObject();
    return x;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
