/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/Types.h"
#include "eckit/utils/Tokenizer.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

template <class Container>
void test_single() {
    std::string source(":lolo1:lolo2::lolo3");
    Container target;

    Tokenizer parse(":");

    parse(source, target);

    EXPECT(target.size() == 3);

    size_t c = 1;
    for (typename Container::const_iterator i = target.begin(); i != target.end(); ++i, ++c) {
        ostringstream s;
        s << "lolo" << c;
        //        std::cout << *i << " ??? " << s.str() << std::endl;
        EXPECT(*i == s.str());
    }
}

//----------------------------------------------------------------------------------------------------------------------

template <class Container>
void test_multi() {
    Container target;

    Tokenizer parse("-:;");

    std::string source1("-lolo0-lolo1-lolo2;lolo3:-lolo4-");
    parse(source1, target);

    EXPECT(target.size() == 5);

    size_t c = 0;
    for (typename Container::const_iterator i = target.begin(); i != target.end(); ++i, ++c) {
        ostringstream s;
        s << "lolo" << c;
        //        std::cout << *i << " ??? " << s.str() << std::endl;
        EXPECT(*i == s.str());
    }

    std::string source2("-lolo5-lolo6-lolo7;lolo8:lolo9-");
    parse(source2, target);

    EXPECT(target.size() == 10);

    c = 0;
    for (typename Container::const_iterator i = target.begin(); i != target.end(); ++i, ++c) {
        ostringstream s;
        s << "lolo" << c;
        //        std::cout << *i << " ??? " << s.str() << std::endl;
        EXPECT(*i == s.str());
    }
}

//----------------------------------------------------------------------------------------------------------------------

void test_keep_empty_list() {
    StringList target;

    Tokenizer parse(":", true);

    std::string source1(":4i:2100:::01:::::.");
    parse(source1, target);

    Log::info() << target << std::endl;

    EXPECT(target.size() == 11);
}

//----------------------------------------------------------------------------------------------------------------------

void test_keep_empty_set() {
    StringSet target;

    Tokenizer parse(":", true);

    std::string source1(":4i:2100:::01:::::.");
    parse(source1, target);

    Log::info() << target << std::endl;

    EXPECT(target.size() == 5);  // 5 uniqe elments
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Test Tokenizer StringList") {

    test_single<StringList>();
    test_multi<StringList>();
    test_keep_empty_list();
}

CASE("Test Tokenizer StringSet") {

    test_single<StringSet>();
    test_multi<StringSet>();
    test_keep_empty_set();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
