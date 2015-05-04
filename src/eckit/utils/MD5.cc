/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"

#include "eckit/utils/MD5.h"

namespace eckit {

MD5::MD5() 
{ 
	md5_init(&s_);
}

MD5::MD5(const digest_t& s)
{
	md5_init(&s_);
	add( s.c_str(), s.size() );
}

MD5::MD5(const void* data, size_t len)
{
	md5_init(&s_);
	add( data, len );
}

MD5::~MD5() {}

void MD5::add(const void* buffer, long length)
{
    if(length > 0) {
        md5_add(&s_,static_cast<const unsigned char*>(buffer), length);
        if(!digest_.empty())
            digest_ = digest_t(); // reset the digest
    }
}

void MD5::add(const int& e)     { add(&e,sizeof(e)); }
void MD5::add(const long& e)    { add(&e,sizeof(e)); }
void MD5::add(const size_t& e)  { add(&e,sizeof(e)); }
void MD5::add(const double& e)  { add(&e,sizeof(e)); }

void MD5::add(const std::string& e) { add(e.c_str(),e.size()); }

void MD5::add(const MD5& md5) { add(md5.digest()); }

MD5::operator std::string()
{
    return digest();
}

MD5::digest_t MD5::digest() const {

  if(digest_.empty()) // recompute the digest
  {
    char digest[33];
    md5_end(&s_,digest);
    digest[32] = 0;
    digest_ = digest;
  }
  return digest_;
}

static unsigned long r[] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

static unsigned long k[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

static const unsigned long t = 32;

//static unsigned long rotate(unsigned long x,unsigned long c) { return (x << c) | (x >> (t-c)); }

//static unsigned long F(unsigned long x,unsigned long y,unsigned long z) { return (x&y)|((~x)&z); }
//static unsigned long G(unsigned long x,unsigned long y,unsigned long z) { return (x&z)|(y&(~z)); }
//static unsigned long H(unsigned long x,unsigned long y,unsigned long z) { return x^y^z;          }
//static unsigned long I(unsigned long x,unsigned long y,unsigned long z) { return y^(x|(~z));     }

#define ROT(x,c) ((x << c) | (x >> (32-c)))

#define _F(x,y,z) ((x&y)|((~x)&z))
#define _G(x,y,z) ((x&z)|(y&(~z)))
#define _H(x,y,z) (x^y^z)
#define _I(x,y,z) (y^(x|(~z)))

#define F_(A,B,C,D,g,i) A += _F(B,C,D)+w[g]+k[i]; A &= 0xffffffff; A = ROT(A, r[i]); A+=B;
#define G_(A,B,C,D,g,i) A += _G(B,C,D)+w[g]+k[i]; A &= 0xffffffff; A = ROT(A, r[i]); A+=B;
#define H_(A,B,C,D,g,i) A += _H(B,C,D)+w[g]+k[i]; A &= 0xffffffff; A = ROT(A, r[i]); A+=B;
#define I_(A,B,C,D,g,i) A += _I(B,C,D)+w[g]+k[i]; A &= 0xffffffff; A = ROT(A, r[i]); A+=B;

void MD5::md5_flush(State* s)
{

//    unsigned long i, g;
    unsigned long a = s->h0;
    unsigned long b = s->h1;
    unsigned long c = s->h2;
    unsigned long d = s->h3;
//    unsigned long f;
//    unsigned long temp;
    unsigned long *w = &s->words[0];
//    unsigned long h;

#if 1
    F_(a,b,c,d, 0, 0);
    F_(d,a,b,c, 1, 1);
    F_(c,d,a,b, 2, 2);
    F_(b,c,d,a, 3, 3);

    F_(a,b,c,d, 4, 4);
    F_(d,a,b,c, 5, 5);
    F_(c,d,a,b, 6, 6);
    F_(b,c,d,a, 7, 7);

    F_(a,b,c,d, 8, 8);
    F_(d,a,b,c, 9, 9);
    F_(c,d,a,b,10,10);
    F_(b,c,d,a,11,11);

    F_(a,b,c,d,12,12);
    F_(d,a,b,c,13,13);
    F_(c,d,a,b,14,14);
    F_(b,c,d,a,15,15);

    G_(a,b,c,d, 1,16);
    G_(d,a,b,c, 6,17);
    G_(c,d,a,b,11,18);
    G_(b,c,d,a, 0,19);

    G_(a,b,c,d, 5,20);
    G_(d,a,b,c,10,21);
    G_(c,d,a,b,15,22);
    G_(b,c,d,a, 4,23);

    G_(a,b,c,d, 9,24);
    G_(d,a,b,c,14,25);
    G_(c,d,a,b, 3,26);
    G_(b,c,d,a, 8,27);

    G_(a,b,c,d,13,28);
    G_(d,a,b,c, 2,29);
    G_(c,d,a,b, 7,30);
    G_(b,c,d,a,12,31);

    H_(a,b,c,d, 5,32);
    H_(d,a,b,c, 8,33);
    H_(c,d,a,b,11,34);
    H_(b,c,d,a,14,35);

    H_(a,b,c,d, 1,36);
    H_(d,a,b,c, 4,37);
    H_(c,d,a,b, 7,38);
    H_(b,c,d,a,10,39);

    H_(a,b,c,d,13,40);
    H_(d,a,b,c, 0,41);
    H_(c,d,a,b, 3,42);
    H_(b,c,d,a, 6,43);

    H_(a,b,c,d, 9,44);
    H_(d,a,b,c,12,45);
    H_(c,d,a,b,15,46);
    H_(b,c,d,a, 2,47);

    I_(a,b,c,d, 0,48);
    I_(d,a,b,c, 7,49);
    I_(c,d,a,b,14,50);
    I_(b,c,d,a, 5,51);

    I_(a,b,c,d,12,52);
    I_(d,a,b,c, 3,53);
    I_(c,d,a,b,10,54);
    I_(b,c,d,a, 1,55);

    I_(a,b,c,d, 8,56);
    I_(d,a,b,c,15,57);
    I_(c,d,a,b, 6,58);
    I_(b,c,d,a,13,59);

    I_(a,b,c,d, 4,60);
    I_(d,a,b,c,11,61);
    I_(c,d,a,b, 2,62);
    I_(b,c,d,a, 9,63);

#else
    for(i=0; i< 16; i++) {
        f = F(b,c,d);
        g = i;
        temp = d;
        d = c;
        c = b;
        h = a + f + k[i] + w[g];
        b = b + rotate(h , r[i]);
        a = temp;
    }

    for(i=16; i< 32; i++) {
        f = G(b,c,d);
        g = (5*i + 1) % 16;
        temp = d;
        d = c;
        c = b;
        h = a + f + k[i] + w[g];
        b = b + rotate(h , r[i]);
        a = temp;
    }
    for(i=32; i< 48; i++) {
        f = H(b,c,d);
        g = (3*i + 5) % 16;
        temp = d;
        d = c;
        c = b;
        h = a + f + k[i] + w[g];
        b = b + rotate(h , r[i]);
        a = temp;
    }
    for(i=48; i< 64; i++) {
        f = I(b,c,d);
        g = (7*i) % 16;
        temp = d;
        d = c;
        c = b;
        h = a + f + k[i] + w[g];
        b = b + rotate(h, r[i]);
        a = temp;
    }

#endif

    s->h0 += a;
    s->h1 += b;
    s->h2 += c;
    s->h3 += d;

    s->word_count = 0;
}

void MD5::md5_init(State* s) {

    memset(s,0,sizeof(State));

    s->h0 = 0x67452301;
    s->h1 = 0xefcdab89;
    s->h2 = 0x98badcfe;
    s->h3 = 0x10325476;
}

void MD5::md5_add(State* s,const void* data,size_t len) {

    unsigned char* p = (unsigned char*)data;
    s->size += len;

    while(len-- > 0) {

        s->bytes[s->byte_count++] = *p++;

        if(s->byte_count == 4) {
            s->words[s->word_count++] = (s->bytes[3]<<24)|(s->bytes[2]<<16)|(s->bytes[1]<<8)|(s->bytes[0]);
            s->byte_count = 0;

            if(s->word_count == 16)
                md5_flush(s);
        }

    }
}

void MD5::md5_end(State* s, char *digest)
{
    uint64_t h = 8;
    uint64_t bits, leng = s->size * h;
    unsigned char c = 0x80;
    int i;

    md5_add(s,&c,1);

    bits = s->size * h;
    c = 0;
    while( (bits % 512) != 448) 
    {
        md5_add(s,&c,1);
        bits = s->size * h;
    }

    for(i = 0; i < 8 ; i++) {
        c =  leng & 0xff;
        leng >>= 8;
        md5_add(s,&c,1);
    }

	typedef unsigned int ui;
    sprintf(digest, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            (ui) (s->h0 & 0xff), (ui) ((s->h0 >> 8) & 0xff), (ui) ((s->h0 >> 16) & 0xff), (ui) ((s->h0 >> 24) & 0xff),
            (ui) (s->h1 & 0xff), (ui) ((s->h1 >> 8) & 0xff), (ui) ((s->h1 >> 16) & 0xff), (ui) ((s->h1 >> 24) & 0xff),
            (ui) (s->h2 & 0xff), (ui) ((s->h2 >> 8) & 0xff), (ui) ((s->h2 >> 16) & 0xff), (ui) ((s->h2 >> 24) & 0xff),
			(ui) (s->h3 & 0xff), (ui) ((s->h3 >> 8) & 0xff), (ui) ((s->h3 >> 16) & 0xff), (ui) ((s->h3 >> 24) & 0xff));
}

} // namespace eckit
