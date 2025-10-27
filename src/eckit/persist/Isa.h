/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_persist_Isa_h
#define eckit_persist_Isa_h

#include <string>

class TypeInfo;

namespace eckit {

class Isa {
public:

    Isa* next_;
    TypeInfo* type_;
    Isa(TypeInfo* t, Isa* n) : next_(n), type_(t) {}
    static void add(TypeInfo* t, const std::string&);
    static Isa* get(const std::string&);
};

class Schema {
public:

    virtual ~Schema() {}
    virtual void start(const std::string&, size_t size)                                          = 0;
    virtual void member(const std::string&, size_t size, size_t offset, const std::string& type) = 0;
    virtual void end(const std::string&)                                                         = 0;
};

template <class T>
void _describe(std::ostream& s, int depth, const T& what) {
    what.describe(s, depth);
}

void _describe(std::ostream& s, int depth, int what);
void _describe(std::ostream& s, int depth, unsigned int what);
void _describe(std::ostream& s, int depth, short what);
void _describe(std::ostream& s, int depth, bool what);
void _describe(std::ostream& s, int depth, unsigned short what);
void _describe(std::ostream& s, int depth, long what);
void _describe(std::ostream& s, int depth, long long what);
void _describe(std::ostream& s, int depth, unsigned long long what);
void _describe(std::ostream& s, int depth, unsigned long what);
void _describe(std::ostream& s, int depth, char what);
void _describe(std::ostream& s, int depth, unsigned char what);
void _describe(std::ostream& s, int depth, double what);

void _startClass(std::ostream& s, int depth, const std::string& name);
void _endClass(std::ostream& s, int depth, const std::string& name);
void _startMember(std::ostream& s, int depth, const std::string& name);
void _endMember(std::ostream& s, int depth, const std::string& name);

template <class T>
void _describe(std::ostream& s, int depth, const std::string& name, const T& what) {
    _startMember(s, depth, name);
    _describe(s, depth, what);
    _endMember(s, depth, name);
}

}  // namespace eckit

#endif
