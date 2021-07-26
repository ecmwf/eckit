/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/serialisation/Stream.h"

#include <netinet/in.h>
#include <stdint.h>
#include <sys/types.h>

#include <cassert>
#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Log.h"
#include "eckit/maths/Functions.h"
#include "eckit/os/BackTrace.h"
#include "eckit/serialisation/BadTag.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

#if 0
#define T(a, x)                                                    \
    do {                                                           \
        std::cout << "Stream: " << a << " ->  " << x << std::endl; \
    } while (0)
#else
#define T(a, x) /**/
#endif

static const char* tag_names[] = {"0",
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
                                  "large blob"};

const int tag_count = sizeof(tag_names) / sizeof(tag_names[0]);


Stream::Stream() :
    lastTag_(tag_zero), writeCount_(0) {}

void Stream::print(std::ostream& s) const {
    s << name();
}

Stream::~Stream() {}

void Stream::putBytes(const void* buf, long len) {
    writeCount_ += len;
    if (write(buf, len) != len)
        throw WriteError(name());
}

std::ostream& operator<<(std::ostream& out, const Stream& s) {
    s.print(out);
    return out;
}

void Stream::getBytes(void* buf, long len) {
    if (read(buf, len) != len)
        throw ReadError(name());
}


void Stream::putChar(unsigned char p) {
    assert(sizeof(unsigned char) == 1);
    putBytes(&p, 1);
}

unsigned char Stream::getChar() {
    assert(sizeof(unsigned char) == 1);

    char p;
    getBytes(&p, 1);
    return p;
}

void Stream::putLong(unsigned long p) {
    if (sizeof(unsigned long) != 4) {
        if (sizeof(unsigned int) == 4) {
            unsigned int n = htonl(p);
            putBytes(&n, sizeof(n));
        }
        else if (sizeof(unsigned short) == 4) {
            unsigned short s = htons(p);
            putBytes(&s, sizeof(s));
        }
        else
            NOTIMP;
    }
    else {
        p = htonl(p);
        putBytes(&p, sizeof(p));
    }
}

unsigned long Stream::getLong() {
    if (sizeof(unsigned long) != 4) {
        if (sizeof(unsigned int) == 4) {
            unsigned int n;
            getBytes(&n, sizeof(n));
            return ntohl(n);
        }
        else if (sizeof(unsigned short) == 4) {
            unsigned short s;
            getBytes(&s, sizeof(s));
            return ntohs(s);
        }
        else
            NOTIMP;
    }
    else {
        long p;
        getBytes(&p, sizeof(p));
        return ntohl(p);
    }
}

std::ostream& operator<<(std::ostream& s, Stream::tag t) {
    if (t >= 0 && t < tag_count)
        return s << '\'' << tag_names[t] << '\'';
    else
        return s << '\'' << long(t) << '\'';
}

void Stream::badTag(Stream::tag need, Stream::tag got) {
    std::ostringstream os;

    os << "Bad tag found in stream ";
    os << *this;
    os << ". Expecting a " << need << ", got a " << got;

    Log::error() << os.str() << std::endl;
    Log::error() << BackTrace::dump() << std::endl;

    if (got == tag_string) {
        long length = getLong();
        std::string s;
        s.resize(length);
        for (long i = 0; i < length; i++)
            s[i] = getChar();
        Log::error() << "String is " << s << std::endl;
    }

    throw BadTag(os.str());
}

Stream::tag Stream::nextTag() {
    if (lastTag_ != tag_zero) {
        tag t    = lastTag_;
        lastTag_ = tag_zero;
        //      Log::debug() << "Found tag " << t << std::endl;
        return t;
    }

    unsigned char c = 0;
    int len;

    if ((len = read(&c, 1)) == 0) {
        //      Log::debug() << "End of stream" << tag_zero << std::endl;
        return tag_eof;
    }

    if (len == EOF)
        throw ShortFile(name());

    tag t = static_cast<tag>(c);
    //  Log::debug() << "Next tag " << t << std::endl;
    return t;
}

Stream::tag Stream::readTag(Stream::tag need) {
    tag t;

    // Skip any end-of-object lingering

    while ((t = nextTag()) == tag_end_obj)
        ;

    if (t == tag_exception) {
        std::string s;
        (*this) >> s;
        throw RemoteException(s, name());
    }

    if (need != t)
        badTag(need, t);


    return t;
}

void Stream::writeTag(Stream::tag t) {
    // Log::info() << "Stream::writeTag(" << t << ")" << std::endl;
    unsigned char c = static_cast<unsigned char>(t);
    putBytes(&c, 1);
}


Stream& Stream::operator<<(char x) {
    union {
        uint8_t u;
        int8_t s;
    } u;
    T("w char", x);
    u.s = x;
    writeTag(tag_char);
    putChar(u.u);
    return *this;
}

Stream& Stream::operator<<(unsigned char x) {
    T("w unsigned char", x);
    writeTag(tag_unsigned_char);
    putChar(x);
    return *this;
}

Stream& Stream::operator<<(int x) {
    union {
        uint32_t u;
        int32_t s;
    } u;
    T("w int", x);
    u.s = x;
    writeTag(tag_int);
    putLong(u.u);
    return *this;
}

Stream& Stream::operator<<(bool x) {
    T("w bool", x);
    return (*this) << int(x);
}

Stream& Stream::operator<<(unsigned int x) {
    T("w unsigned int", x);
    writeTag(tag_unsigned_int);
    putLong(x);
    return *this;
}

Stream& Stream::operator<<(long x) {
    union {
        uint32_t u;
        int32_t s;
    } u;
    T("w long", x);
    u.s = x;
    writeTag(tag_long);
    putLong(u.u);
    return *this;
}

Stream& Stream::operator<<(unsigned long x) {
    T("w unsigned long", x);
    writeTag(tag_unsigned_long);
    putLong(x);
    return *this;
}

Stream& Stream::operator<<(long long x) {
    union {
        uint64_t u;
        int64_t s;
    } u;
    T("w long long", x);
    u.s = x;
    writeTag(tag_long_long);
    putLong(u.u >> 32);
    putLong(u.u & 0xffffffff);
    return *this;
}

Stream& Stream::operator<<(unsigned long long x) {
    T("w unsigned long long", x);
    writeTag(tag_unsigned_long_long);
    putLong(x >> 32);
    putLong(x & 0xffffffff);
    return *this;
}

Stream& Stream::operator<<(short x) {
    union {
        uint16_t u;
        int16_t s;
    } u;
    T("w short", x);
    u.s = x;
    writeTag(tag_short);
    putLong(u.u);
    return *this;
}

Stream& Stream::operator<<(unsigned short x) {
    T("w unsigned short", x);
    writeTag(tag_unsigned_short);
    putLong(x);
    return *this;
}

Stream& Stream::operator<<(float x) {
    T("w float", x);
    writeTag(tag_float);
    NOTIMP;
    // return *this;
}

union Double {
    double d;
#if __SIZEOF_LONG__ == 4
    struct {
        unsigned long hi;
        unsigned long lo;
    } s;
#else
    struct {
        unsigned int hi;
        unsigned int lo;
    } s;
#endif
};

Stream& Stream::operator<<(double x) {
    T("w double", x);
    writeTag(tag_double);
    Double d;
    ASSERT(sizeof(d.d) == 2 * sizeof(d.s.hi));
    d.d = x;
    putLong(d.s.hi);
    putLong(d.s.lo);
    return *this;
}

Stream& Stream::operator<<(const char* x) {
    T("w std::string", x);
    writeTag(tag_string);
    ASSERT(x);
    long len = strlen(x);
    putLong(len);
    assert(sizeof(unsigned char) == 1);
    putBytes(x, len);
    return *this;
}

Stream& Stream::operator<<(const std::string& x) {
    T("w std::string", x);
    writeTag(tag_string);
    const long len = x.length();
    putLong(len);
    assert(sizeof(unsigned char) == 1);
    putBytes(x.c_str(), len);
    return *this;
}

void Stream::writeLargeBlob(const void* buffer, size_t size) {
    T("w blob", x);
    writeTag(tag_large_blob);

    unsigned long long len = size;
    ASSERT(size_t(len) == size);

    // std::cout << "Stream::writeLargeBlob " << size << std::endl;

    putLong(len >> 32);
    putLong(len & 0xffffffff);

    long n        = 0x80000000;
    const char* p = static_cast<const char*>(buffer);
    while (size > 0) {
        long l = size > size_t(n) ? n : size;
        putBytes(p, l);
        p += l;
        size -= l;
    }
}

void Stream::writeBlob(const void* buffer, size_t size) {
    T("w blob", x);
    writeTag(tag_blob);

    long len = size;
    ASSERT(size_t(len) == size);
    ASSERT(len >= 0);

    putLong(len);
    putBytes(buffer, len);
}

Stream& Stream::operator<<(const Buffer& x) {
    writeBlob(x, x.size());
    return *this;
}

Stream& Stream::operator<<(const std::exception& e) {
    T("w exception", e.what());
    writeTag(tag_exception);
    return *this << std::string(e.what());
}

Stream& Stream::operator>>(char& x) {
    union {
        uint8_t u;
        int8_t s;
    } u;
    readTag(tag_char);
    u.u = getChar();
    x   = u.s;
    T("r char", x);
    return *this;
}

Stream& Stream::operator>>(unsigned char& x) {
    readTag(tag_unsigned_char);
    x = getChar();
    T("r unsigned char", x);
    return *this;
}

Stream& Stream::operator>>(int& x) {
    union {
        uint32_t u;
        int32_t s;
    } u;
    readTag(tag_int);
    u.u = getLong();
    x   = u.s;
    T("r int", x);
    return *this;
}

Stream& Stream::operator>>(bool& x) {
    int n;
    (*this) >> n;
    x = n;
    T("r bool", x);
    return *this;
}

Stream& Stream::operator>>(unsigned int& x) {
    readTag(tag_unsigned_int);
    x = getLong();
    T("r unsigned int", x);
    return *this;
}

Stream& Stream::operator>>(short& x) {
    union {
        uint16_t u;
        int16_t s;
    } u;
    readTag(tag_short);
    u.u = getLong();
    x   = u.s;
    T("r short", x);
    return *this;
}

Stream& Stream::operator>>(unsigned short& x) {
    readTag(tag_unsigned_short);
    x = getLong();
    T("r unsigned short", x);
    return *this;
}

Stream& Stream::operator>>(long& x) {
    union {
        uint32_t u;
        int32_t s;
    } u;
    readTag(tag_long);
    u.u = getLong();
    x   = u.s;
    T("r long", x);
    return *this;
}

Stream& Stream::operator>>(unsigned long& x) {
    readTag(tag_unsigned_long);
    x = getLong();
    T("r unsigned long", x);
    return *this;
}

Stream& Stream::operator>>(long long& x) {
    union {
        uint64_t u;
        int64_t s;
    } u;
    readTag(tag_long_long);
    uint64_t u1 = getLong();
    ;
    uint64_t u2 = getLong();
    u.u         = (u1 << 32) | u2;
    x           = u.s;
    T("r long long", x);
    return *this;
}

Stream& Stream::operator>>(unsigned long long& x) {
    readTag(tag_unsigned_long_long);
    unsigned long long u1 = getLong();
    ;
    unsigned long long u2 = getLong();
    x                     = (u1 << 32) | u2;
    T("r unsigned long long", x);
    return *this;
}


Stream& Stream::operator>>(float& x) {
    readTag(tag_float);
    NOTIMP;
    // T("r float", x);
    // return *this;
}

void Stream::readLargeBlob(void* buffer, size_t size) {
    readTag(tag_large_blob);

    unsigned long long u1 = getLong();
    ;
    unsigned long long u2  = getLong();
    unsigned long long len = (u1 << 32) | u2;

    ASSERT(size_t(len) == size);

    // std::cout << "Stream::readLargeBlob " << size << std::endl;

    long n  = 0x80000000;
    char* p = static_cast<char*>(buffer);
    while (size > 0) {
        long l = size > size_t(n) ? n : size;
        getBytes(p, l);
        p += l;
        size -= l;
    }
}

void Stream::rewind() {
    NOTIMP;
}

void Stream::closeOutput() {
    NOTIMP;
}

void Stream::closeInput() {
    NOTIMP;
}

void Stream::readBlob(void* buffer, size_t size) {
    readTag(tag_blob);
    long len = getLong();
    ASSERT(len >= 0);
    ASSERT(size_t(len) == size);
    getBytes(buffer, len);
    T("r blob", x);
}


size_t Stream::blobSize() {
    readTag(tag_blob);
    long len = getLong();
    ASSERT(len >= 0);
    return len;
}

Stream& Stream::operator>>(Buffer& x) {
    readBlob(x, x.size());
    return *this;
}

Stream& Stream::operator>>(std::map<std::string, std::string>& m) {
    unsigned long size;
    *this >> size;

    m.clear();

    for (unsigned long i = 0; i < size; i++) {
        std::string k;
        *this >> k;
        std::string v;
        *this >> v;
        m[k] = v;
    }
    return *this;
}

Stream& Stream::operator>>(double& x) {
    readTag(tag_double);
    Double d;
    d.s.hi = getLong();
    d.s.lo = getLong();
    x      = d.d;
    T("r double", x);
    return *this;
}

Stream& Stream::operator>>(std::string& s) {
    readTag(tag_string);
    const long length = getLong();

    const long sz = eckit::round(length + 1, 8);  // some padding to avoid zero-length buffer
    char buf[sz];
    getBytes(buf, length);

    s.resize(length);
    s.assign(buf, length);

    T("r std::string", s);
    return *this;
}

bool Stream::next(std::string& s) {
    tag t = nextTag();
    if (t == tag_eof)
        return false;

    if (t != tag_string)
        badTag(tag_string, t);

    long length = getLong();
    char buf[length];
    getBytes(buf, length);

    s.resize(length);
    s.assign(buf, length);

    T("r std::string", s);

    return true;
}

bool Stream::next(int& x) {

    tag t = nextTag();
    if (t == tag_eof)
        return false;

    if (t != tag_int)
        badTag(tag_int, t);

    union {
        uint32_t u;
        int32_t s;
    } u;

    u.u = getLong();
    x   = u.s;
    T("r int", x);
    return true;
}


bool Stream::next(bool& b) {

    int x = 0;
    if (!next(x)) {
        return false;
    }

    b = x;
    T("r bool", b);

    return true;
}


void Stream::startObject() {
    T("w start", 0);
    writeTag(tag_start_obj);
}

void Stream::endObject() {
    T("w end", 0);
    writeTag(tag_end_obj);
}

bool Stream::endObjectFound() {
    lastTag_ = nextTag();
    return (lastTag_ == tag_end_obj) || (lastTag_ == tag_zero);
}

void Stream::skipEndObject() {
    if (endObjectFound())
        lastTag_ = tag_zero;
}

#if 0

void Stream::startRecord(unsigned long type) {
    writeTag(tag_start_rec);
    putLong(type);
}

void Stream::endRecord() {
    writeTag(tag_end_obj);
}

bool Stream::nextRecord(unsigned long &type, bool sync) {
    int first = 0;

    for (;;) {
        unsigned char c = 0;
        int len;

        if ( (len = read(&c, 1)) == 0)
            return false;

        if (len == EOF)
            throw ShortFile(name());

        tag t =  static_cast<tag>(c);

        if (t == tag_start_rec) {
            type = getLong();
            return true;
        }

        if (t != tag_end_rec) {
            if (!sync) badTag(tag_start_rec, t);
            if (first) {
                Log::error() << "Bad tag found in stream " << t << std::endl;
                Log::error() << "Trying to synchronise" << std::endl;
                first = 0;
            }
        }
    }
}

#endif

bool Stream::next() {
    for (;;) {
        tag t = nextTag();

        if (t == tag_start_obj)
            return true;

        if (t == tag_eof)
            return false;

        if (t != tag_end_obj)
            badTag(tag_start_obj, t);
    }
}

class StreamDecoder : public Stream {
    const char* buffer_;
    size_t pos_;
    size_t len_;

    virtual long write(const void* buf, long len) { NOTIMP; }

    virtual long read(void* buf, long len) {
        ::memcpy(buf, buffer_ + pos_, len);
        len_ += len;
        pos_ += len;
        return len;
    }

    virtual std::string name() const { return "StreamDecoder"; }

public:
    StreamDecoder(const char* buffer) :
        buffer_(buffer), pos_(0), len_(0) {}
    size_t len() const { return len_; }
};

void Stream::dump(std::ostream& out, const char* p, size_t len) {
    size_t i = 0;
    while (i < len) {
        if (p[i] < last_tag) {
            out << "<" << tag_names[static_cast<size_t>(p[i])] << ">";

            StreamDecoder d(&p[i]);

            switch (p[i]) {
                case tag_zero:
                    NOTIMP;
                case tag_start_obj:
                    NOTIMP;
                case tag_end_obj:
                    NOTIMP;
                case tag_char:
                    NOTIMP;
                case tag_unsigned_char:
                    NOTIMP;
                case tag_int: {
                    int s;
                    d >> s;
                    out << s;
                }; break;
                case tag_unsigned_int:
                    NOTIMP;
                case tag_short:
                    NOTIMP;
                case tag_unsigned_short:
                    NOTIMP;
                case tag_long:
                    NOTIMP;
                case tag_unsigned_long:
                    NOTIMP;
                case tag_long_long:
                    NOTIMP;
                case tag_unsigned_long_long: {
                    unsigned long long s;
                    d >> s;
                    out << s;
                }; break;
                case tag_float:
                    NOTIMP;
                case tag_double:
                    NOTIMP;

                case tag_string: {
                    std::string s;
                    d >> s;
                    out << s;
                }; break;

                case tag_blob:
                    NOTIMP;
                case tag_exception:
                    NOTIMP;
                case tag_start_rec:
                    NOTIMP;
                case tag_end_rec:
                    NOTIMP;
                case tag_eof:
                    NOTIMP;
                default:
                    NOTIMP;
            }

            i += d.len();
        }
        else {
            out << "<invalid>";
            i++;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
