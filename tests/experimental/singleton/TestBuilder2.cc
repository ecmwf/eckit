// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "TestFactory.h"

class TestBuilder2 : public TestBuilder {
public:

    void build() { std::cout << "building a b2" << std::endl; }
};

//----------------------------------------------------------------------------------------------------------------------

AutoRegistBuilder<TestBuilder2> builder2("b2");
