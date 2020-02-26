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
#include "eckit/config/Resource.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/net/UDPClient.h"
#include "eckit/runtime/Report.h"
#include "eckit/thread/ThreadSingleton.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Reporter : public NonCopyable {
public:  // methods
    static Reporter& instance();

    Reporter();

    ~Reporter() = default;

    bool enabled() { return not clients_.empty(); }

    void monitor(const std::string& domain, const std::string& msg);

private:  // methods
    void broadcast(const char *buf, size_t length);


private:  // members
    std::vector<std::unique_ptr<net::UDPClient>> clients_;
};

//----------------------------------------------------------------------------------------------------------------------

Reporter& Reporter::instance() {
    static ThreadSingleton<Reporter> s;
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

void Reporter::monitor(const std::string& domain, const std::string& msg) {

        //        time_t timestamp  = ::time(0);

        std::ostringstream out;
        JSON j(out);
        j.startObject();
        j << domain;
        j << msg;
        j.endObject();

        std::string str = out.str();

//        ECKIT_DEBUG_VAR(str)

        broadcast(str.data(), str.size());
}

void Reporter::broadcast(const char* buf, size_t length) {
    for (auto& c : clients_) {
        c->send(buf, length);
    }
}


//----------------------------------------------------------------------------------------------------------------------

bool Report::active() {
    static bool reporting = eckit::Resource<bool>("reporting", true);
    return reporting;
}

void Report::monitor(const std::string& domain, const std::string& msg) {
    Reporter::instance().monitor(domain, msg);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
