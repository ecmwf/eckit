// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/config/Resource.h"
#include "eckit/net/Endpoint.h"
#include "eckit/runtime/Application.h"

using namespace eckit;

class Test : public Application {
    virtual void run();

public:

    Test(int argc, char** argv) : Application(argc, argv, "HOME") {}
};


void Test::run() {
    net::Endpoint addr(Resource<std::string>("-addr;$ADDR;addr", "localhost:9000"));

    Log::info() << addr << std::endl;
}

int main(int argc, char** argv) {
    Test app(argc, argv);
    app.start();
    return 0;
}
