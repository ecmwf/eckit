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

#include "eckit/cmd/Arg.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Application.h"

using namespace eckit;

class Test : public Application {
    virtual void run();

public:
    Test(int argc, char** argv) :
        Application(argc, argv, "HOME") {}
};


void Test::run() {
    Arg a(((Arg("-abc", Arg::text) | Arg("-cde", Arg::number) | Arg("-def", Arg::tape)) | (Arg("-abc", Arg::text), Arg("-cde", Arg::number), Arg("-def", Arg::tape))));

    Log::info() << a << std::endl;
}

int main(int argc, char** argv) {
    Test app(argc, argv);
    app.start();
    return 0;
}
