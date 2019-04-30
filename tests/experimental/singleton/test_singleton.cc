/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "TestFactory.h"

int main(int argc, char** argv) {
    TestBuilder& b1 = TestFactory::instance().get("b1");
    TestBuilder& b2 = TestFactory::instance().get("b2");

    b1.build();
    b2.build();

    return 0;
}
