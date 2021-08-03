/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/runtime/Pipe.h"
#include "eckit/runtime/Application.h"
#include "eckit/utils/Translator.h"

using namespace eckit;

class Test : public Application {
    virtual void run();

public:
    Test(int argc, char** argv) :
        Application(argc, argv, "HOME") {}
};

static void reader(Pipe<std::string>& p) {
    Log::info() << "start reader" << std::endl;
    while (!p.eof()) {
        std::string& s = p.receive();
        Log::info() << s << std::endl;
    }
    Log::info() << "end reader" << std::endl;
}

static void writer(Pipe<std::string>& p) {
    Log::info() << "start writer" << std::endl;
    for (int i = 0; i < 30; i++) {
        std::string& s = p.message();
        s              = std::string("Hello ") + Translator<int, std::string>()(i);
        p.send();
    }
    Log::info() << "end writer" << std::endl;
}

void Test::run() {
    Pipe<std::string> p;
    p.spawn(&::reader);
    writer(p);
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    Test app(argc, argv);
    app.start();
    return 0;
}
