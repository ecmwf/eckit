/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_machine_h
#define eckit_machine_h

#include "ecbuild_config.h"

#define _(A) A

//-----------------------------------------------------------------------------

/* POSIX */

#include <string.h>

//-----------------------------------------------------------------------------

/* STL */

#include <set>
#include <map>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <memory>
#include <string>
#include <numeric>

#include <iostream>
#include <iterator>
#include <iomanip>
#include <fstream>
#include <new>
#include <sstream>

using namespace std;

//-----------------------------------------------------------------------------

/* some defines for 64 bit functions */

#if (!defined EC_HAVE_OFF64T) && (defined EC_HAVE_OFFT)
#define off64_t     off_t
#endif

#if (!defined EC_HAVE_STAT64) && (defined EC_HAVE_STAT)
#if (defined EC_HAVE_STRUCT_STAT64)
#error "system has struct stat64 but no function stat64"
#endif
#define  stat64    stat
#endif

#if (!defined EC_HAVE_FSTAT64) && (defined EC_HAVE_FSTAT)
#define  fstat64    fstat
#endif

#if (!defined EC_HAVE_FTRUNCATE64) && (defined EC_HAVE_FTRUNCATE)
#define ftruncate64     ftruncate
#endif
#if (!defined EC_HAVE_FLOCK64) && (defined EC_HAVE_FLOCK)
#define flock64  flock
#endif
#if (!defined EC_HAVE_LSEEK64) && (defined EC_HAVE_LSEEK)
#define lseek64  lseek
#endif
#if (!defined EC_HAVE_OPEN64) && (defined EC_HAVE_OPEN)
#define open64  open
#endif
#if (!defined EC_HAVE_FOPEN64) && (defined EC_HAVE_FOPEN)
#define fopen64  fopen
#endif
#if (!defined EC_HAVE_FSEEKO64) && (defined EC_HAVE_FSEEKO)
#define fseeko64  fseeko
#endif
#if (!defined EC_HAVE_FTELLO64) && (defined EC_HAVE_FTELLO)
#define ftello64  ftello
#endif
#if (!defined EC_HAVE_MMAP64) && (defined EC_HAVE_MMAP)
#define mmap64      mmap
#endif

#if (!defined EC_HAVE_AIO64) && (defined EC_HAVE_AIO)
#define  aio_read64    aio_read
#define  aio_write64   aio_write
#define  aio_suspend64 aio_suspend
#define  aio_fsync64   aio_fsync
#define  aio_error64   aio_error
#define  aio_return64  aio_return
#define  aiocb64       aiocb
#endif

#if (!defined EC_HAVE_STRUCT_STATVFS64) && (defined EC_HAVE_STRUCT_STATVFS)
#define  statvfs64    statvfs
#endif

#if (!defined EC_HAVE_F_GETLK64) && (defined EC_HAVE_F_GETLK)
#define F_GETLK64   F_GETLK
#endif
#if (!defined EC_HAVE_F_SETLK64) && (defined EC_HAVE_F_SETLK)
#define F_SETLK64   F_SETLK
#endif
#if (!defined EC_HAVE_F_SETLKW64) && (defined EC_HAVE_F_SETLKW)
#define F_SETLKW64   F_SETLKW
#endif

#if (!defined EC_HAVE_MAP_ANONYMOUS) && (defined EC_HAVE_MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif

#if (!defined EC_HAVE_FDATASYNC) && (defined EC_HAVE_FSYNC)
#define fdatasync  fsync
#endif

#ifndef EC_HAVE_FUNCTION_DEF
#define __FUNCTION__ ""
#endif

/* Usefull macros */

#ifndef NUMBER
#define NUMBER(x) (sizeof(x)/sizeof(x[0]))
#endif

//-----------------------------------------------------------------------------

#ifdef __GNUC__ /* GCC gets confused about offsetof */

/// @todo eckit fixme
// static double _offset; // on macosx ?

static char _offset_dummy[80];
static void* _offset = &_offset_dummy;

#define member_offset(Z,z)  size_t( reinterpret_cast<char*>(&reinterpret_cast<Z*>(_offset)->z) - reinterpret_cast<char*>(_offset))
#define member_size(Z,z)    size_t( sizeof(reinterpret_cast<Z*>(_offset)->z))

static void* keep_gcc_quiet_about_offset_2(void* d);
static void* keep_gcc_quiet_about_offset_1(void* d) { return keep_gcc_quiet_about_offset_2(_offset); }
static void* keep_gcc_quiet_about_offset_2(void* d) { return keep_gcc_quiet_about_offset_1(_offset); }

#endif /* __GNUC__ */

#ifndef member_size
#define member_size(a,b)   size_t(sizeof(((a*)0)->b))
#endif

#ifndef member_offset
#define member_offset(a,b) size_t(&(((a*)0)->b))
#endif

//-----------------------------------------------------------------------------

class TypeInfo;

//-----------------------------------------------------------------------------

namespace eckit {

template<bool b> struct compile_assert {};
template<>       struct compile_assert<true> { static void check() {} };


struct output_iterator {
  typedef output_iterator_tag iterator_category;
  typedef void                value_type;
  typedef void                difference_type;
  typedef void                pointer;
  typedef void                reference;
};

template<class T>
inline void zero(T& p) { ::memset(&p,0,sizeof(T)); }
class Bless {
public:

    bool          operator() (bool*  a)          { return *a; }
    int           operator() (int*   a)          { return *a; }
    short         operator() (short*   a)          { return *a; }
    char          operator() (char*  a)          { return *a; }
    long          operator() (long* a)           { return *a; }
    long long     operator() (long long* a)      { return *a; }

    unsigned long operator() (unsigned long* a) { return *a; }
    unsigned int  operator() (unsigned int* a)  { return *a; }
    unsigned char operator() (unsigned char* a) { return *a; }
    unsigned short operator() (unsigned short* a) { return *a; }
    unsigned long long operator() (unsigned long long* a)      { return *a; }

    double        operator() (double* a)        { return *a; }

    template<class T>
    Bless&        operator() (T*)               { return *this; }
};

class Exporter;
class Evolve {

public:

    Evolve(eckit::Exporter&);
    Evolve(Evolve*,const char*, const char*);

    Evolve operator() (const char*, const char* = 0);

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

    const string& path() const { return path_; }

private:

    Exporter&      e_;
    string         path_;
    Evolve*        parent_;

};

class Isa {
public:
    Isa* next_;
    TypeInfo* type_;
    Isa(TypeInfo* t,Isa* n) : next_(n), type_(t) {}
    static void add(TypeInfo* t,const string&);
    static Isa* get(const string&);

};

class Schema {
public:
    virtual void start(const string&, size_t size) = 0;
    virtual void member(const string&, size_t size, size_t offset, const string& type) = 0;
    virtual void end(const string&) = 0;
};

template<class T>
void _describe(ostream& s,int depth,const T& what)
{
    what.describe(s,depth);
}

void _describe(ostream& s,int depth,int what);
void _describe(ostream& s,int depth,unsigned int what);
void _describe(ostream& s,int depth,short what);
void _describe(ostream& s,int depth,bool what);
void _describe(ostream& s,int depth,unsigned short what);
void _describe(ostream& s,int depth,long what);
void _describe(ostream& s,int depth,long long what);
void _describe(ostream& s,int depth,unsigned long long what);
void _describe(ostream& s,int depth,unsigned long what);
void _describe(ostream& s,int depth,char what);
void _describe(ostream& s,int depth,unsigned char what);
void _describe(ostream& s,int depth,double what);

void _startClass(ostream& s,int depth,const string& name);
void _endClass(ostream& s,int depth,const string& name);
void _startMember(ostream& s,int depth,const string& name);
void _endMember(ostream& s,int depth,const string& name);

template<class T>
void _describe(ostream& s,int depth,const string& name,const T& what)
{
    _startMember(s,depth,name);
    _describe(s,depth,what);
    _endMember(s,depth,name);
}

class Exporter;

void _startObject(eckit::Exporter&,unsigned long long type, unsigned long long location, unsigned long long id, size_t count);
void _endObject(eckit::Exporter&,unsigned long long type, unsigned long long location, unsigned long long id, size_t count);
void _startSubObject(eckit::Exporter&);
void _endSubObject(eckit::Exporter&);
void _nextSubObject(eckit::Exporter&);
void _startClass(eckit::Exporter&,const string& name);
void _endClass(eckit::Exporter&,const string& name);
void _startClass(eckit::Exporter&,const char* name);
void _endClass(eckit::Exporter&,const char* name);
void _startMember(eckit::Exporter&,const char* name);
void _endMember(eckit::Exporter&,const char* name);

//-----------------------------------------------------------------------------

template<class T>
void _export(eckit::Exporter& h,const T& what)
{
    what._export(h);
}

void _export(eckit::Exporter&,int what);
void _export(eckit::Exporter&,unsigned int what);
void _export(eckit::Exporter&,short what);
void _export(eckit::Exporter&,bool what);
void _export(eckit::Exporter&,unsigned short what);
void _export(eckit::Exporter&,long what);
void _export(eckit::Exporter&,long long what);
void _export(eckit::Exporter&,unsigned long long what);
void _export(eckit::Exporter&,unsigned long what);
void _export(eckit::Exporter&,char what);
void _export(eckit::Exporter&,unsigned char what);
void _export(eckit::Exporter&,double what);

template<class T>
void _export(eckit::Exporter& s ,const char* name,const T& what)
{
    _startMember(s,name);
    _export(s,what);
    _endMember(s,name);
}

//-----------------------------------------------------------------------------

} // namespace eckit

//-----------------------------------------------------------------------------

void* operator new(size_t,void* addr, eckit::Evolve&);

//-----------------------------------------------------------------------------

#endif

