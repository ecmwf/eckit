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


#include <iostream>
#include <memory>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/testing/Test.h"


int main(int argc, char* argv[]) {
    std::unique_ptr<eckit::Configuration> param(
        new eckit::MappedConfiguration({{"a", -123}, {"b", "B"}, {"c", 123UL}}));

    int a = 0;
    EXPECT(param->get("a", a));
    std::cout << "a: '" << a << "'" << std::endl;

    std::string b;
    EXPECT(param->get("b", b));
    std::cout << "b: '" << b << "'" << std::endl;

    size_t c = 0;
    EXPECT(param->get("c", c));
    std::cout << "c: '" << c << "'" << std::endl;

    int d = 321;
    dynamic_cast<eckit::MappedConfiguration*>(param.get())->set("b", d);
    EXPECT(param->get("b", d));
    std::cout << "d: '" << d << "'" << std::endl;
}
