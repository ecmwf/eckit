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
#include <fstream>
#include <memory>

#include "eckit/config/LocalConfiguration.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/net/UDPClient.h"
#include "eckit/runtime/Stats.h"
#include "eckit/thread/ThreadSingleton.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Reporter : public NonCopyable {
public:
    static Reporter& instance();

    Reporter();

    ~Reporter() = default;

    bool enabled() { return not clients_.empty(); }

    void broadcast(void* buf, long length);

    void report(const std::string& domain, const std::string& msg);

private:  // members
    std::vector<std::unique_ptr<net::UDPClient>> clients_;
};

//----------------------------------------------------------------------------------------------------------------------

Reporter& Reporter::instance() {
    ECKIT_DEBUG_HERE
    static ThreadSingleton<Reporter> s;
    ECKIT_DEBUG_HERE
    return s.instance();
}

Reporter::Reporter() {
    PathName path = "~/etc/config/stats.yaml";

    ECKIT_DEBUG_VAR(path)

    if (not path.exists())
        return;

    YAMLConfiguration config(path);

    ECKIT_DEBUG_VAR(config)

    for (auto& cfg : config.getSubConfigurations("servers")) {
        clients_.emplace_back(new net::UDPClient(cfg));
    }

    /// TODO MAKE A TEMPLATE HEADER
    pid_t pid        = getpid();
    pthread_t thread = pthread_self();
}

void Reporter::report(const std::string& domain, const std::string& msg) {
    if (enabled()) {

        //        time_t timestamp  = ::time(0);

        std::ostringstream out;
        JSON j(out);
        j.startObject();
        j << domain;
        j << msg;
        j.endObject();

        std::string str = out.str();

        ECKIT_DEBUG_VAR(str)

        broadcast((void*) str.data(), str.size());
    }
}

void Reporter::broadcast(void* buf, long length) {
    for (auto& c : clients_) {
        c->send(buf, length);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Stats::report(const std::string& domain, const std::string& msg) {
    Reporter::instance().report(domain, msg);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
