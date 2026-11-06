// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/web/JavaAgent.h"

#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/os/Password.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


eckit::ClassSpec JavaAgent::classSpec_ = {
    &Streamable::classSpec(),
    "JavaAgent",
};
Reanimator<JavaAgent> JavaAgent::reanimator_;

void JavaAgent::encode(eckit::Stream& s) const {
    Streamable::encode(s);
}

JavaAgent::JavaAgent(eckit::Stream& s) : Streamable(s), stream_(s) {}

JavaAgent::~JavaAgent() {}

void JavaAgent::startObject(const std::string& clss) {
    stream_.startObject();
    stream_ << clss;
}

void JavaAgent::endObject() {
    stream_.endObject();
}

void JavaAgent::serve(eckit::Stream& s, std::istream& in, std::ostream& out) {
    std::unique_ptr<JavaAgent> a(Reanimator<JavaAgent>::reanimate(s));
    ASSERT(a.get());

    Log::info() << *a << std::endl;

    int agentClearance = a->clearance();

    if (agentClearance == JavaAgent::none) {
        s << int(0);  // No need for password
    }
    else {
        Log::status() << "Authenticating for " << *a << std::endl;
        s << int(1);  // Ask for password;

        std::string user;
        std::string password;

        s >> user;
        s << Password::salt(user);
        s >> password;

        if (!Password::check(user, password)) {
            Log::status() << "Authentication failed for " << user << ", " << *a << std::endl;
            s << int(-1);
            return;
        }
        s << int(0);

        a->user_ = user;
    }

    Log::status() << "Executing " << *a << std::endl;
    a->execute(s, in, out);
    Log::status() << "Completed " << *a << std::endl;
}

template <>
Streamable* eckit::Reanimator<JavaAgent>::ressucitate(eckit::Stream& s) const {
    return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
