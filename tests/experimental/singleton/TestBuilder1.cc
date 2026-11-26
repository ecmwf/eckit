// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "TestFactory.h"

class TestBuilder1 : public TestBuilder {
public:

    void build() { std::cout << "building a b1" << std::endl; }
};

//----------------------------------------------------------------------------------------------------------------------

AutoRegistBuilder<TestBuilder1> builder1("b1");
