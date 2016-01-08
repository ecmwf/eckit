/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"
#include "eckit/types/Types.h"

#include "eckit/container/Cache.h"

using namespace std;
using namespace eckit;

namespace eckit_test {

class TestCache : public Tool {
public:

    TestCache(int argc,char **argv): Tool(argc,argv) {}

    ~TestCache() {}

    virtual void run();

    void test_contructor();
    void test_expire();
    void test_purge();
    void test_update();
    void test_fetch();
    void test_insert();

};

//-----------------------------------------------------------------------------

struct Obj
{
    Obj() : s_(),d_(0) {}
    Obj(const string& s, const unsigned long long& d) : s_(s),d_(d) {}
    string s_;
    unsigned long long d_;
    friend std::ostream& operator<<(std::ostream& s,const Obj& x) { s << x.s_ << ":" << x.d_; return s; }    
};

//-----------------------------------------------------------------------------

void TestCache::test_contructor()
{
    Cache<string,Obj> cache;
    
    ASSERT( cache.insert("a", Obj("aaa",11111) ) );
    ASSERT( cache.insert("b", Obj("bbb",22222) ) );
    ASSERT( cache.insert("c", Obj("ccc",33333) ) );
    
//    cache.print(std::cout);
    
    ASSERT( cache.valid("a") );
    ASSERT( cache.valid("b") );
    ASSERT( cache.valid("c") );
    
    cache.clear();

    ASSERT( ! cache.valid("a") );
    ASSERT( ! cache.valid("b") );
    ASSERT( ! cache.valid("c") );
}

//-----------------------------------------------------------------------------

void TestCache::test_expire()
{
    Cache<string,Obj> cache;
    
    ASSERT( cache.insert("a", Obj("aaa",11111) ) );
    ASSERT( cache.insert("b", Obj("bbb",22222) ) );
    ASSERT( cache.insert("c", Obj("ccc",33333) ) );
    
    cache.expire("b");

//    cache.print(std::cout);
    
    ASSERT(  cache.size() == 3 );
    ASSERT(  cache.valid("a") );
    ASSERT( !cache.valid("b") );
    ASSERT(  cache.valid("c") );
}

//-----------------------------------------------------------------------------

void TestCache::test_purge()
{
    Cache<string,Obj> cache;
    
    ASSERT( cache.insert("a", Obj("aaa",11111) ) );
    ASSERT( cache.insert("b", Obj("bbb",22222) ) );
    ASSERT( cache.insert("c", Obj("ccc",33333) ) );
    ASSERT( cache.insert("d", Obj("ddd",44444) ) );
    
    ASSERT(  cache.size() == 4 );

    ASSERT( cache.expire("b") );
    ASSERT( cache.expire("d") );
    
    cache.purge();
    
    cache.print(std::cout);

    ASSERT(  cache.size() == 2 );
    
    ASSERT(  cache.valid("a") );
    ASSERT( !cache.valid("b") );
    ASSERT(  cache.valid("c") );
    ASSERT( !cache.valid("d") );
}

//-----------------------------------------------------------------------------

void TestCache::test_update()
{
    Cache<string,Obj> cache;
    
    ASSERT( cache.insert("a", Obj("aaa",11111) ) );
    ASSERT( cache.insert("b", Obj("bbb",22222) ) );
    ASSERT( cache.insert("c", Obj("ccc",33333) ) );

    Obj o1( "ddd", 44444);

    ASSERT( !cache.update("d",o1) );

    Obj o2( "BBB", 2);
    
    ASSERT( cache.update("b",o2) );
    
    Obj o3;
    
    ASSERT( cache.fetch("d",o3) );
    
    ASSERT( o3.s_ == "ddd"  );
    ASSERT( o3.d_ ==  44444 );

    ASSERT( cache.fetch("b",o3) );
    
    ASSERT( o3.s_ == "BBB"  );
    ASSERT( o3.d_ ==  2     );
}

//-----------------------------------------------------------------------------

void TestCache::test_fetch()
{
    Cache<string,Obj> cache;
    
    ASSERT( cache.insert("a", Obj("aaa",11111) ) );
    ASSERT( cache.insert("b", Obj("bbb",22222) ) );
    ASSERT( cache.insert("c", Obj("ccc",33333) ) );
    ASSERT( cache.insert("d", Obj("ddd",44444) ) );

    Obj o;

    ASSERT( !cache.fetch("f",o) ); // no obj f exists
    
    ASSERT( cache.fetch("b",o) );  // obj b exists

    ASSERT( o.s_ == "bbb" );
    ASSERT( o.d_ ==  22222  );
}

//-----------------------------------------------------------------------------

void TestCache::test_insert()
{
    Cache<string,Obj> cache;
    
    ASSERT( cache.insert("a", Obj("aaa",11111) ) );
    ASSERT( cache.insert("b", Obj("bbb",22222) ) );
    ASSERT( cache.insert("c", Obj("ccc",33333) ) );

    // double insert fails
    
    ASSERT( !cache.insert("a", Obj("AAA",1) ) ); 

    Obj o;
    
    ASSERT( cache.fetch("a",o) );
    ASSERT( o.s_ != "AAA" );
    ASSERT( o.d_ !=  1    );
    ASSERT( o.s_ == "aaa" );
    ASSERT( o.d_ ==  11111);
}

//-----------------------------------------------------------------------------
            
void TestCache::run()
{
    test_contructor();
    test_expire();
    test_purge();
    test_update();
    test_fetch();
    test_insert();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestCache app(argc,argv);
    return app.start();
}
