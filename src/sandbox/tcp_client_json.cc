/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <sstream>

#include "eckit/log/JSON.h"
#include "eckit/net/TCPClient.h"
#include "eckit/runtime/Application.h"


using namespace eckit;

class Client : public Application {

    virtual void run();

public:
    Client(int argc, char** argv) :
        Application(argc, argv, "HOME") {}
};


class XX {

    std::string app_;
    std::string event_;

    long step_;

public:  // methods
    XX(const std::string& app) :
        app_(app), step_(0) {}

    void step(long step) { step_ = step; }
    void event(const std::string& e) { event_ = e; }

    void json(JSON& s) const {
        s.startObject();
        s << "event" << event_;
        s << "step" << step_;
        s << "app" << app_;
        s.endObject();
    }
};

void Client::run() {
    std::string remoteHost = "localhost";

    XX x("IFS");
    x.event("iteration");

    for (auto s : {0, 3, 6, 9, 12}) {

        ::sleep(3);

        net::TCPClient c;
        c.connect(remoteHost, 10000);

        std::ostringstream os;
        JSON msg(os);

        x.step(s);
        x.json(msg);

        Log::info() << "sending " << os.str() << std::endl;


        c.write(os.str().c_str(), os.str().size());
    }
}


int main(int argc, char** argv) {
    Client app(argc, argv);
    app.start();
}
