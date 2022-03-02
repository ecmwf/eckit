/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>
#include <iostream>
#include <memory>

#include "eckit/utils/Optional.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

Optional<long> none() {
    return Optional<long>();
}
Optional<long> number() {
    return Optional<long>(2);
}
Optional<std::vector<long>> array() {
    std::vector<long> out;
    for (int i=0; i<10; i++) {
        out.emplace_back(i);
    }
    return Optional<std::vector<long>>(std::move(out));
}

CASE("value") {
    {
        auto ret = none();
        ASSERT(!ret);
    }
    {
        auto ret = number();
        ASSERT(ret);

        long m = ret.value();
        ASSERT(m == 2);

        ret.value() = 4;
        const long l = ret.value();
        ASSERT(l == 4);
    }
    {
        auto ret = array();
        ASSERT(ret);
        for (int i=0; i<10; i++) {
            ret()[i]++;
        }
        ASSERT(ret()[2] == 3);
    }
}

CASE("wrap") {
    {
        Optional<long> opt;
        ASSERT(!opt);

        opt = 3;
        ASSERT(opt);
        ASSERT(opt() == 3);

        auto ret = array();
        ASSERT(ret);
        for (int i=0; i<10; i++) {
            ret()[i]++;
        }
        Optional<std::vector<long>> out(std::move(ret()));
        ASSERT(out);
        ASSERT(out()[2] == 3);

        std::vector<long>& tmp = out();
        ASSERT(tmp[3] == 4);

        tmp[3] = 5;
        ASSERT(out()[3] == 5);

        std::vector<long> tmp2 = out();
        ASSERT(tmp2[3] == 5);

        tmp2[3] = 4;
        ASSERT(out()[3] == 5);
    }

}

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
