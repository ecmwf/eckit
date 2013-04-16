/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <netinet/in.h>
#include <assert.h>

#include "eclib/Buffer.h"
#include "eckit/log/Log.h"
#include "eclib/StrStream.h"
#include "eclib/Stream.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

#define UNSIGNED(type,var) ((unsigned type)(var))
#define SIGNED(type,var)   ((type)(var))
 
#if 0
#define T(a,x) do { cout << "Stream: " << a << " ->  "  << x << endl;} while(0)
#else
#define T(a,x) /**/
#endif

static const char *tag_names[] = {
    "0",
    "start of object",
    "end of object",
    "char",
    "unsigned char",
    "int",
    "unsigned int",
    "short",
    "unsigned short",
    "long",
    "unsigned long",
    "long long",
    "unsigned long long",
    "float",
    "double",
    "string",
    "blob",
    "exception",
    "start of record",
    "end of record",
    "end of file",
};

const int tag_count = sizeof(tag_names)/sizeof(tag_names[0]);


Stream::Stream():
    lastTag_(tag_zero),
    writeCount_(0)
{
}

Stream::~Stream()
{

}

void Stream::putBytes(const void* buf,long len)
{
    writeCount_ += len;
    if(write(buf,len) != len)
        throw WriteError(name());
}

void Stream::getBytes(void* buf,long len)
{
    if(read(buf,len) != len)
        throw ReadError(name());
}


void Stream::putChar(unsigned char p)
{
    assert(sizeof(unsigned char) == 1);
    putBytes(&p,1);
}

unsigned char Stream::getChar()
{
    assert(sizeof(unsigned char) == 1);

    char p;
    getBytes(&p,1);
    return p;
}

void Stream::putLong(unsigned long p)
{

    if(sizeof(unsigned long) != 4)
    {
        if( sizeof(unsigned int) == 4 )
        {
            unsigned int n = htonl(p);
            putBytes(&n,sizeof(n));
        }
        else if(sizeof(unsigned short) == 4)
        {
            unsigned short s = htons(p);
            putBytes(&s,sizeof(s));
        }
        else NOTIMP;
    }
    else
    {
        p = htonl(p);
        putBytes(&p,sizeof(p));
    }
}

unsigned long Stream::getLong()
{
    if(sizeof(unsigned long) != 4)
    {
        if( sizeof(unsigned int) == 4 )
        {
            unsigned int n;
            getBytes(&n,sizeof(n));
            return ntohl(n);
        }
        else if(sizeof(unsigned short) == 4)
        {
            unsigned short s;
            getBytes(&s,sizeof(s));
            return ntohs(s);
        }
        else NOTIMP;
    }
    else
    {
        long p;
        getBytes(&p,sizeof(p));
        return ntohl(p);
    }
}

ostream& operator<<(ostream& s,Stream::tag t)
{
    if(t >= 0 && t < tag_count)
        return s << '\'' << tag_names[t] << '\'';
    else
        return s << '\'' << long(t) << '\'';
}

void Stream::badTag(Stream::tag need,Stream::tag got)
{
    StrStream os;

    os << "Bad tag found in stream ";
    os << name();
    os << ". Expecting a " << need << ", got a " << got << StrStream::ends;

    Log::error() << string(os) << endl;

    if(got == tag_string)
    {
        long length = getLong();
        string s;
        s.resize(length);
        for(long i=0;i<length;i++) s[i] = getChar();
        Log::error() << "String is " << s << endl;
    }

    throw BadTag(string(os));
}

Stream::tag Stream::nextTag()
{
    if(lastTag_ != tag_zero)
    {
        tag t    = lastTag_;
        lastTag_ = tag_zero;
        //		Log::debug() << "Found tag " << t << endl;
        return t;
    }

    unsigned char c = 0;
    int len;

    if( (len = read(&c,1)) == 0)
    {
        //      Log::debug() << "End of stream" << tag_zero << endl;
        return tag_eof;
    }

    if(len == EOF)
        throw ShortFile(name());

    tag t = static_cast<tag>(c);
    //  Log::debug() << "Next tag " << t << endl;
    return t;

}

Stream::tag Stream::readTag(Stream::tag need)
{
    tag t;

    // Skip any end-of-object lingering

    while((t = nextTag())  == tag_end_obj)
        ;

    if(t == tag_exception)
    {
        string s;
        (*this) >> s;
        throw RemoteException(s, name());
    }

    if(need != t)
        badTag(need,t);


    return t;
}

void Stream::writeTag(Stream::tag t)
{
    // Log::info() << "Stream::writeTag(" << t << ")" << endl;
    unsigned char c = static_cast<unsigned char>(t);
    putBytes(&c,1);
}


Stream& Stream::operator<<(char x)
{
    T("w char",x);
    writeTag(tag_char);
    putChar(UNSIGNED(char,x));
    return *this;
}

Stream& Stream::operator<<(unsigned char x)
{
    T("w unsigned char",x);
    writeTag(tag_unsigned_char);
    putChar(x);
    return *this;
}

Stream& Stream::operator<<(int x)
{
    T("w int",x);
    writeTag(tag_int);
    putLong(UNSIGNED(int,x));
    return *this;
}

Stream& Stream::operator<<(bool x)
{
    T("w bool",x);
    return (*this) << int(x);
}

Stream& Stream::operator<<(unsigned int x)
{
    T("w unsigned int",x);
    writeTag(tag_unsigned_int);
    putLong(x);
    return *this;
}

Stream& Stream::operator<<(long x)
{
    T("w long",x);
    writeTag(tag_long);
    putLong(UNSIGNED(long,x));
    return *this;
}

Stream& Stream::operator<<(unsigned long x)
{
    T("w unsigned long",x);
    writeTag(tag_unsigned_long);
    putLong(x);
    return *this;
}

Stream& Stream::operator<<(long long x)
{
    T("w long long",x);
    writeTag(tag_long_long);
    unsigned long long ull = UNSIGNED(long long,x);
    putLong(ull >> 32);
    putLong(ull & 0xffffffff);
    return *this;
}

Stream& Stream::operator<<(unsigned long long x)
{
    T("w unsigned long long",x);
    writeTag(tag_unsigned_long_long);
    putLong(x >> 32);
    putLong(x & 0xffffffff);
    return *this;
}

Stream& Stream::operator<<(short x)
{
    T("w short",x);
    writeTag(tag_short);
    putLong(UNSIGNED(short,x));
    return *this;
}

Stream& Stream::operator<<(unsigned short x)
{
    T("w unsigned short",x);
    writeTag(tag_unsigned_short);
    putLong(x);
    return *this;
}

Stream& Stream::operator<<(float x)
{
    T("w float",x);
    writeTag(tag_float);
    NOTIMP;
    return *this;
}

union Double {
    double d;
#if __SIZEOF_LONG__ == 4
    struct { unsigned long hi; unsigned long lo; } s;
#else
    struct { unsigned int hi; unsigned int lo; } s;
#endif
};

Stream& Stream::operator<<(double x)
{
    T("w double",x);
    writeTag(tag_double);
    Double d;
    ASSERT(sizeof(d.d) == 2*sizeof(d.s.hi));
    d.d = x;
    putLong(d.s.hi);
    putLong(d.s.lo);
    return *this;
}

Stream& Stream::operator<<(const char* x)
{
    T("w string",x);
    writeTag(tag_string);
    ASSERT(x);
    long len = strlen(x);
    putLong(len);
    assert(sizeof(unsigned char) == 1);
    putBytes(x,len);
    return *this;
}

Stream& Stream::operator<<(const string& x)
{
    T("w string",x);
    writeTag(tag_string);
    long len = x.length();
    putLong(len);

    char buf[len];
    assert(sizeof(unsigned char) == 1);
    for(long i=0;i<len;i++)
        buf[i] = x[i];

    putBytes(buf,len);
    return *this;
}

Stream& Stream::operator<<(const Buffer& x)
{
    T("w blob",x);
    writeTag(tag_blob);
    long len = x.size();
    putLong(len);
    putBytes(x,len);
    return *this;
}

Stream& Stream::operator<<(const exception& e)
{
    T("w exception",e.what());
    writeTag(tag_exception);
    return *this << string(e.what());
}

Stream& Stream::operator>>(char& x)
{
    readTag(tag_char);
    unsigned char c = getChar();
    x = SIGNED(char,c);
    T("r char",x);
    return *this;
}

Stream& Stream::operator>>(unsigned char& x)
{
    readTag(tag_unsigned_char);
    x = getChar();
    T("r unsigned char",x);
    return *this;
}

Stream& Stream::operator>>(int& x)
{
    readTag(tag_int);
    unsigned long p = getLong();
    x = SIGNED(int,p);
    T("r int",x);
    return *this;
}

Stream& Stream::operator>>(bool& x)
{
    int n; (*this) >> n; x = n;
    T("r bool",x);
    return *this;
}

Stream& Stream::operator>>(unsigned int& x)
{
    readTag(tag_unsigned_int);
    x = getLong();
    T("r unsigned int",x);
    return *this;
}

Stream& Stream::operator>>(short& x)
{
    readTag(tag_short);
    unsigned long p = getLong();
    x = SIGNED(short,p);
    T("r short",x);
    return *this;
}

Stream& Stream::operator>>(unsigned short& x)
{
    readTag(tag_unsigned_short);
    x = getLong();
    T("r unsigned short",x);
    return *this;
}

Stream& Stream::operator>>(long& x)
{
    readTag(tag_long);
    unsigned long p = getLong();
    x = SIGNED(long,p);
    T("r long",x);
    return *this;
}

Stream& Stream::operator>>(unsigned long& x)
{
    readTag(tag_unsigned_long);
    x = getLong();
    T("r unsigned long",x);
    return *this;
}

Stream& Stream::operator>>(long long& x)
{
    readTag(tag_long_long);
    unsigned long long u1 = getLong();;
    unsigned long long u2 = getLong();
    unsigned long long ull = (u1 << 32) | u2;
    x = SIGNED(long long,ull);
    T("r long long",x);
    return *this;
}

Stream& Stream::operator>>(unsigned long long& x)
{
    readTag(tag_unsigned_long_long);
    unsigned long long u1 = getLong();;
    unsigned long long u2 = getLong();
    x = (u1 << 32) | u2;
    T("r unsigned long long",x);
    return *this;
}


Stream& Stream::operator>>(float& x)
{
    readTag(tag_float);
    NOTIMP;
    T("r float",x);
    return *this;
}

Stream& Stream::operator>>(Buffer& x)
{
    readTag(tag_blob);
    long size = getLong();;
    ASSERT(size <= x.size());
    getBytes(x, size);
    T("r blob",x);
    return *this;
}

Stream& Stream::operator>>(double& x)
{
    readTag(tag_double);
    Double d;
    d.s.hi = getLong();
    d.s.lo = getLong();
    x = d.d;
    T("r double",x);
    return *this;
}

Stream& Stream::operator>>(string& s)
{
    readTag(tag_string);
    long length = getLong();
    char buf[length];
    getBytes(buf,length);

    s.resize(length);

    for(long i=0;i<length;i++)
        s[i] = buf[i];

    T("r string",s);
    return *this;
}

bool Stream::next(string& s)
{
    tag t = nextTag();
    if(t == tag_eof)
        return false;

    if(t != tag_string)
        badTag(tag_string,t);

    long length = getLong();
    char buf[length];
    getBytes(buf,length);

    s.resize(length);

    for(long i=0;i<length;i++)
        s[i] = buf[i];

    T("r string",s);

    return true;
}

void Stream::startObject()
{
    T("w start",0);
    writeTag(tag_start_obj);
}

void Stream::endObject()
{
    T("w end",0);
    writeTag(tag_end_obj);
}

bool Stream::endObjectFound()
{
    lastTag_ = nextTag();
    return (lastTag_ == tag_end_obj) || (lastTag_ == tag_zero);
}

void Stream::skipEndObject()
{
    if(endObjectFound()) lastTag_ = tag_zero;
}

#if 0

void Stream::startRecord(unsigned long type)
{
    writeTag(tag_start_rec);
    putLong(type);
}

void Stream::endRecord()
{
    writeTag(tag_end_obj);
}

bool Stream::nextRecord(unsigned long& type,bool sync)
{
    int first = 0;

    for(;;)
    {
        unsigned char c = 0;
        int len;

        if( (len = read(&c,1)) == 0)
            return false;

        if(len == EOF)
            throw ShortFile(name());

        tag t =  static_cast<tag>(c);

        if(t == tag_start_rec)
        {
            type = getLong();
            return true;
        }

        if(t != tag_end_rec)
        {
            if(!sync) badTag(tag_start_rec,t);
            if(first)
            {
                Log::error() << "Bad tag found in stream " << t << endl;
                Log::error() << "Trying to synchronise" << endl;
                first = 0;
            }
        }
    }
}

#endif

bool Stream::next()
{
    for(;;)
    {
        tag t = nextTag();

        if(t == tag_start_obj)
            return true;

        if(t == tag_eof)
            return false;

        if(t != tag_end_obj)
            badTag(tag_start_obj,t);
    }
}

class StreamDecoder : public Stream {
    const char* buffer_;
    size_t pos_;
    size_t len_;

    virtual long write(const void* buf,long len) { NOTIMP; }

    virtual long read(void* buf,long len) {
        ::memcpy(buf,buffer_ + pos_,len);
        len_ += len;
        pos_ += len;
        return len;
    }

    virtual string name() const { return "StreamDecoder"; }

public:
    StreamDecoder(const char* buffer): buffer_(buffer), pos_(0), len_(0) {}
    size_t len() const { return len_; }

};

void Stream::dump(ostream& out,const char* p, size_t len)
{
    size_t i = 0;
    while(i < len)
    {
        if(p[i] < last_tag) {
            out << "<" << tag_names[p[i]] << ">";

            StreamDecoder d(&p[i]);

            switch(p[i]) {
                case tag_zero: NOTIMP;break;
                case tag_start_obj: NOTIMP;break;
                case tag_end_obj: NOTIMP;break;
                case tag_char: NOTIMP;break;
                case tag_unsigned_char: NOTIMP;break;
                case tag_int:
                                        {
                                     int s;
                                     d >> s;
                                     out << s;
                                        };
                                        break;
                case tag_unsigned_int: NOTIMP;break;
                case tag_short: NOTIMP;break;
                case tag_unsigned_short: NOTIMP;break;
                case tag_long: NOTIMP;break;
                case tag_unsigned_long: NOTIMP;break;
                case tag_long_long: NOTIMP;break;
                case tag_unsigned_long_long:
                                    {
                                     unsigned long long s;
                                     d >> s;
                                     out << s;
                                    };
                                    break;
                case tag_float: NOTIMP;break;
                case tag_double: NOTIMP;break;

                case tag_string:
                                 {
                                     string s;
                                     d >> s;
                                     out << s;
                                 };
                                 break;

                case tag_blob: NOTIMP;break;
                case tag_exception: NOTIMP;break;
                case tag_start_rec: NOTIMP;break;
                case tag_end_rec: NOTIMP;break;
                case tag_eof: NOTIMP;break;
                default: NOTIMP;break;
            }

            i +=  d.len();
        }
        else {
            out << "<invalid>";
        i++;
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace eckit

