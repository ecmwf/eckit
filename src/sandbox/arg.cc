// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "eckit/cmd/Arg.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Application.h"

using namespace eckit;

class Test : public Application {
    virtual void run();

public:

    Test(int argc, char** argv) : Application(argc, argv, "HOME") {}
};


void Test::run() {
    Arg a(((Arg("-abc", Arg::text) | Arg("-cde", Arg::number) | Arg("-def", Arg::tape)) |
           (Arg("-abc", Arg::text), Arg("-cde", Arg::number), Arg("-def", Arg::tape))));

    Log::info() << a << std::endl;
}

int main(int argc, char** argv) {
    Test app(argc, argv);
    app.start();
    return 0;
}
