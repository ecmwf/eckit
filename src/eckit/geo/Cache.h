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


#include <string>
#include <type_traits>


namespace eckit::geo::cache {


class Cache {
public:
    virtual ~Cache()    = default;
    virtual void call() = 0;
};


class ClassA;
class ClassB;


template <typename T>
class CacheableT : public Cache {
    T obj;

public:
    CacheableT(T&& o) :
        obj(std::move(o)) {}

    void call() override {
        if constexpr (std::is_same<T, ClassA>::value) {
            obj.functionA();
            return;
        }

        if constexpr (std::is_same<T, ClassB>::value) {
            obj.functionB();
            return;
        }

        return;
    }
};


}  // namespace eckit::geo::cache
