/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Cache.h"


#include <map>
#include <memory>
#include <utility>


namespace eckit::geo::cache {


static std::map<std::string, std::unique_ptr<Cache>> __cache;


class ClassA {
public:
    void functionA() const {}
};


class ClassB {
public:
    void functionB() const {}
};


void test() {
    __cache.insert({"a", std::make_unique<CacheableT<ClassA>>(ClassA())});
    __cache.insert({"b", std::make_unique<CacheableT<ClassB>>(ClassB())});

    __cache["b"]->call();
}


}  // namespace eckit::geo::cache
