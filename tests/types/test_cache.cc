/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"
#include "eckit/container/Cache.h"
#include "eckit/log/Log.h"
#include "eckit/types/Types.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

struct Obj {
    Obj() :
        s_(), d_(0) {}
    Obj(const string& s, const unsigned long long& d) :
        s_(s), d_(d) {}
    string s_;
    unsigned long long d_;
    friend std::ostream& operator<<(std::ostream& s, const Obj& x) {
        s << x.s_ << ":" << x.d_;
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

CASE("test_contructor") {
    Cache<string, Obj> cache;

    EXPECT(cache.insert("a", Obj("aaa", 11111)));
    EXPECT(cache.insert("b", Obj("bbb", 22222)));
    EXPECT(cache.insert("c", Obj("ccc", 33333)));

    //    cache.print(std::cout);

    EXPECT(cache.valid("a"));
    EXPECT(cache.valid("b"));
    EXPECT(cache.valid("c"));

    cache.clear();

    EXPECT(!cache.valid("a"));
    EXPECT(!cache.valid("b"));
    EXPECT(!cache.valid("c"));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_expire") {
    Cache<string, Obj> cache;

    EXPECT(cache.insert("a", Obj("aaa", 11111)));
    EXPECT(cache.insert("b", Obj("bbb", 22222)));
    EXPECT(cache.insert("c", Obj("ccc", 33333)));

    cache.expire("b");

    //    cache.print(std::cout);

    EXPECT(cache.size() == 3);
    EXPECT(cache.valid("a"));
    EXPECT(!cache.valid("b"));
    EXPECT(cache.valid("c"));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_purge") {
    Cache<string, Obj> cache;

    EXPECT(cache.insert("a", Obj("aaa", 11111)));
    EXPECT(cache.insert("b", Obj("bbb", 22222)));
    EXPECT(cache.insert("c", Obj("ccc", 33333)));
    EXPECT(cache.insert("d", Obj("ddd", 44444)));

    EXPECT(cache.size() == 4);

    EXPECT(cache.expire("b"));
    EXPECT(cache.expire("d"));

    cache.purge();

    cache.print(std::cout);

    EXPECT(cache.size() == 2);

    EXPECT(cache.valid("a"));
    EXPECT(!cache.valid("b"));
    EXPECT(cache.valid("c"));
    EXPECT(!cache.valid("d"));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_update") {
    Cache<string, Obj> cache;

    EXPECT(cache.insert("a", Obj("aaa", 11111)));
    EXPECT(cache.insert("b", Obj("bbb", 22222)));
    EXPECT(cache.insert("c", Obj("ccc", 33333)));

    Obj o1("ddd", 44444);

    EXPECT(!cache.update("d", o1));

    Obj o2("BBB", 2);

    EXPECT(cache.update("b", o2));

    Obj o3;

    EXPECT(cache.fetch("d", o3));

    EXPECT(o3.s_ == "ddd");
    EXPECT(o3.d_ == 44444);

    EXPECT(cache.fetch("b", o3));

    EXPECT(o3.s_ == "BBB");
    EXPECT(o3.d_ == 2);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_fetch") {
    Cache<string, Obj> cache;

    EXPECT(cache.insert("a", Obj("aaa", 11111)));
    EXPECT(cache.insert("b", Obj("bbb", 22222)));
    EXPECT(cache.insert("c", Obj("ccc", 33333)));
    EXPECT(cache.insert("d", Obj("ddd", 44444)));

    Obj o;

    EXPECT(!cache.fetch("f", o));  // no obj f exists

    EXPECT(cache.fetch("b", o));  // obj b exists

    EXPECT(o.s_ == "bbb");
    EXPECT(o.d_ == 22222);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_insert") {
    Cache<string, Obj> cache;

    EXPECT(cache.insert("a", Obj("aaa", 11111)));
    EXPECT(cache.insert("b", Obj("bbb", 22222)));
    EXPECT(cache.insert("c", Obj("ccc", 33333)));

    // double insert fails

    EXPECT(!cache.insert("a", Obj("AAA", 1)));

    Obj o;

    EXPECT(cache.fetch("a", o));
    EXPECT(o.s_ != "AAA");
    EXPECT(o.d_ != 1);
    EXPECT(o.s_ == "aaa");
    EXPECT(o.d_ == 11111);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
