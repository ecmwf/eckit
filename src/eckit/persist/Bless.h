/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_persist_Bless_h
#define eckit_persist_Bless_h

#include "eckit/persist/Isa.h"

//--------------------------------------------------------------------------------------------------

#ifdef __GNUC__ /* GCC gets confused about offsetof */

static char _offset_dummy[80];
static void* _offset = &_offset_dummy;

#define member_offset(Z, z) \
    size_t(reinterpret_cast<char*>(&reinterpret_cast<Z*>(_offset)->z) - reinterpret_cast<char*>(_offset))
#define member_size(Z, z) size_t(sizeof(reinterpret_cast<Z*>(_offset)->z))

namespace eckit {
namespace {
static void* keep_gcc_quiet_about_offset_2(void* d);
static void* keep_gcc_quiet_about_offset_1(void*) {
    return keep_gcc_quiet_about_offset_2(_offset);
}
static void* keep_gcc_quiet_about_offset_2(void*) {
    return keep_gcc_quiet_about_offset_1(_offset);
}
}  // namespace
}  // namespace eckit

#endif /* __GNUC__ */

#ifndef member_size
#define member_size(a, b) size_t(sizeof(((a*)0)->b))
#endif

#ifndef member_offset
#define member_offset(a, b) size_t(&(((a*)0)->b))
#endif

//--------------------------------------------------------------------------------------------------

namespace eckit {

class Exporter;

//--------------------------------------------------------------------------------------------------

class Bless {
public:

    bool operator()(bool* a) { return *a; }
    int operator()(int* a) { return *a; }
    short operator()(short* a) { return *a; }
    char operator()(char* a) { return *a; }
    long operator()(long* a) { return *a; }
    long long operator()(long long* a) { return *a; }

    unsigned long operator()(unsigned long* a) { return *a; }
    unsigned int operator()(unsigned int* a) { return *a; }
    unsigned char operator()(unsigned char* a) { return *a; }
    unsigned short operator()(unsigned short* a) { return *a; }
    unsigned long long operator()(unsigned long long* a) { return *a; }

    double operator()(double* a) { return *a; }

    template <class T>
    Bless& operator()(T*) {
        return *this;
    }
};

//--------------------------------------------------------------------------------------------------

class Evolve {

public:

    Evolve(eckit::Exporter&);
    Evolve(Evolve*, const char*, const char*);

    Evolve operator()(const char*, const char* = nullptr);

    operator bool();
    operator double();

    operator int();
    operator short();
    operator char();
    operator long();
    operator long long();

    operator unsigned int();
    operator unsigned short();
    operator unsigned char();
    operator unsigned long();
    operator unsigned long long();

    const std::string& path() const { return path_; }

private:

    Exporter& e_;
    std::string path_;
    Evolve* parent_;
};

void _startObject(eckit::Exporter&, unsigned long long type, unsigned long long location, unsigned long long id,
                  size_t count);
void _endObject(eckit::Exporter&, unsigned long long type, unsigned long long location, unsigned long long id,
                size_t count);
void _startSubObject(eckit::Exporter&);
void _endSubObject(eckit::Exporter&);
void _nextSubObject(eckit::Exporter&);
void _startClass(eckit::Exporter&, const std::string& name);
void _endClass(eckit::Exporter&, const std::string& name);
void _startClass(eckit::Exporter&, const char* name);
void _endClass(eckit::Exporter&, const char* name);
void _startMember(eckit::Exporter&, const char* name);
void _endMember(eckit::Exporter&, const char* name);

//--------------------------------------------------------------------------------------------------

template <class T>
void _export(eckit::Exporter& h, const T& what) {
    what._export(h);
}

void _export(eckit::Exporter&, int what);
void _export(eckit::Exporter&, unsigned int what);
void _export(eckit::Exporter&, short what);
void _export(eckit::Exporter&, bool what);
void _export(eckit::Exporter&, unsigned short what);
void _export(eckit::Exporter&, long what);
void _export(eckit::Exporter&, long long what);
void _export(eckit::Exporter&, unsigned long long what);
void _export(eckit::Exporter&, unsigned long what);
void _export(eckit::Exporter&, char what);
void _export(eckit::Exporter&, unsigned char what);
void _export(eckit::Exporter&, double what);

template <class T>
void _export(eckit::Exporter& s, const char* name, const T& what) {
    _startMember(s, name);
    _export(s, what);
    _endMember(s, name);
}

//--------------------------------------------------------------------------------------------------

}  // namespace eckit

void* operator new(size_t, void* addr, eckit::Evolve&);

#endif
