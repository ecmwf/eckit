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

class TestBuilder2 : public TestBuilder {
public:
    void build() { std::cout << "building a b2" << std::endl; }
};

//----------------------------------------------------------------------------------------------------------------------

AutoRegistBuilder<TestBuilder2> builder2("b2");
