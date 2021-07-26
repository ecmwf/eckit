/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/testing/Test.h"

#include "eckit/value/Value.h"

#include <algorithm>  // std::reverse
#include <vector>     // std::vector

using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {


//----------------------------------------------------------------------------------------------------------------------
// Types follow a strict ordering as follows:
//      bool [ true > false ] > number > string > nil > list > map > Date > Time > DateTime > orderedmap
//----------------------------------------------------------------------------------------------------------------------

CASE("Sequential comparisons are correct") {

    // Total order between types:
    EXPECT(Value(true) > Value(false));
    EXPECT(Value(false) > Value((int)1));
    EXPECT(Value((int)2) > Value(std::string("foo")));
    EXPECT(Value(std::string("foo")) > Value());
    EXPECT(Value() > Value::makeList());
    EXPECT(Value::makeList() > Value::makeMap());
    EXPECT(Value::makeMap() > Value(Date(2016, 5, 1)));
    EXPECT(Value(Date(2016, 5, 1)) > Value(Time(1000)));
    EXPECT(Value(Time(1000)) > Value(DateTime()));
    EXPECT(Value(DateTime()) > Value::makeOrderedMap());
}

CASE("Every type compares correctly against every other type") {

    // Vector of value types in ascending order (smallest first)
    std::vector<Value> values;
    values.push_back(Value::makeOrderedMap());
    values.push_back(Value(DateTime()));
    values.push_back(Value(Time(1000)));
    values.push_back(Value(Date(2016, 5, 1)));
    values.push_back(Value::makeList());
    values.push_back(Value());
    values.push_back(Value(std::string("foo")));
    values.push_back(Value(std::string("goo")));
    values.push_back(Value((int)1));
    values.push_back(Value((double)1.5));
    values.push_back(Value((int)2));
    values.push_back(Value(false));
    values.push_back(Value(true));

    size_t sanity_check = 0;

    for (size_t i = 0; i < values.size(); i++) {
        for (size_t j = 0; j < values.size(); j++) {

            Log::info() << "Compare " << values[i] << " (" << values[i].typeName() << ")"
                        << " vs. " << values[j] << " (" << values[j].typeName() << ")" << std::endl;

            // ------------------

            if (i < j) {

                EXPECT(values[i] < values[j]);
                EXPECT(!(values[i] >= values[j]));
                EXPECT(values[i].compare(values[j]) == -1);

                EXPECT(values[j] >= values[i]);
                EXPECT(!(values[j] < values[i]));
                EXPECT(values[j].compare(values[i]) == 1);

                sanity_check++;
            }
            else {  // j <= i

                EXPECT(values[j] <= values[i]);
                EXPECT(!(values[j] > values[i]));
                EXPECT(values[j].compare(values[i]) <= 0);

                EXPECT(values[i] >= values[j]);
                EXPECT(!(values[i] < values[j]));
                EXPECT(values[i].compare(values[j]) >= 0);
            }

            // ------------------

            if (i == j) {

                EXPECT(values[i] == values[j]);
                EXPECT(!(values[i] != values[j]));
                EXPECT(values[i].compare(values[j]) == 0);

                EXPECT(values[j] == values[i]);
                EXPECT(!(values[j] != values[i]));
                EXPECT(values[j].compare(values[i]) == 0);

                sanity_check++;
            }
            else {  // i != j

                EXPECT(values[i] != values[j]);
                EXPECT(!(values[i] == values[j]));
                EXPECT(values[i].compare(values[j]) != 0);

                EXPECT(values[j] != values[i]);
                EXPECT(!(values[j] == values[i]));
                EXPECT(values[j].compare(values[i]) != 0);
            }

            // ------------------

            if (i > j) {

                EXPECT(values[i] > values[j]);
                EXPECT(!(values[i] <= values[j]));
                EXPECT(values[i].compare(values[j]) == 1);

                EXPECT(values[j] <= values[i]);
                EXPECT(!(values[j] > values[i]));
                EXPECT(values[j].compare(values[i]) == -1);

                sanity_check++;
            }
            else {  // i <= j

                EXPECT(values[i] <= values[j]);
                EXPECT(!(values[i] > values[j]));
                EXPECT(values[i].compare(values[j]) <= 0);

                EXPECT(values[j] >= values[i]);
                EXPECT(!(values[j] < values[i]));
                EXPECT(values[j].compare(values[i]) >= 0);
            }

            // ------------------
        }
    }

    EXPECT(sanity_check == values.size() * values.size());  // Checks that one of '<', '==' or '>' was triggered for each pair
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
