// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "TestFactory.h"

int main(int argc, char** argv) {
    TestBuilder& b1 = TestFactory::instance().get("b1");
    TestBuilder& b2 = TestFactory::instance().get("b2");

    b1.build();
    b2.build();

    return 0;
}
