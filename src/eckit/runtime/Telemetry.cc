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
#include "eckit/runtime/Main.h"
#include "eckit/runtime/Telemetry.h"
#include "eckit/thread/ThreadSingleton.h"

namespace eckit {
namespace runtime {

//----------------------------------------------------------------------------------------------------------------------

Report::~Report() {}

JSON& operator<<(eckit::JSON& s, const Report& p) {
    p.json(s);
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

const char* report_type_name[] = {
    [Report::APPSTART] = "appstart",
    [Report::APPSTOP]  = "appstop",
    [Report::INFO]     = "info",
    [Report::METER]    = "meter",
    [Report::COUNTER]  = "counter",
};

const char* report_type_to_name(Report::Type t) {
    if (t < Report::MAXREPORT)
        return report_type_name[t];
    /* otherwise */
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

class Reporter : public NonCopyable {
public:
    static Reporter& instance();
    static int version() { return 1; }

    Reporter();

    ~Reporter() = default;

    bool enabled() { return not clients_.empty(); }

    void broadcast(void* buf, long length);

    void report(Report::Type type, const Report& p);

private:
    std::vector<std::unique_ptr<net::UDPClient>> clients_;  //< list of clients to contact servers

    std::string service_type;
    std::string service_name;
    std::string node_name;
};

//----------------------------------------------------------------------------------------------------------------------

Reporter& Reporter::instance() {
    static ThreadSingleton<Reporter> s;
    return s.instance();
}

Reporter::Reporter() {

    PathName path = "~/etc/config/telemetry.yaml";

    if (not path.exists())
        return;

    YAMLConfiguration config(path);

    ECKIT_DEBUG_VAR(config)

    service_type = config.getString("service_type", "unknown");
    service_name = config.getString("service_name", "unknown");
    node_name    = Resource<std::string>("node", "unknown");  // same as in NodeInfo

    for (auto& cfg : config.getSubConfigurations("servers")) {
        clients_.emplace_back(new net::UDPClient(cfg));
    }
}

void Reporter::report(Report::Type type, const Report& report) {

    if (not enabled())  //< early return if we don't have servers configured
        return;

    std::ostringstream out;
    JSON j(out);
    j.startObject();

    j << "version" << version();
    j << "type" << report_type_to_name(type);
    j << "service_type" << service_type;
    j << "service_name" << service_name;
    j << "node" << node_name;
    j << "application" << Main::instance().name();
    j << "hostname" << Main::hostname();
    j << "pid" << int(::getpid());
    j << "thread" << (unsigned long)::pthread_self();
    j << "time" << ::time(nullptr);

    j << "report";
    {
        j.startObject();
        j << report;
        j.endObject();
    }

    j.endObject();

    std::string msg = out.str();

    ECKIT_DEBUG_VAR(msg)

    broadcast((void*)msg.data(), msg.size());
}

void Reporter::broadcast(void* buf, long length) {
    for (auto& c : clients_) {
        c->send(buf, length);
    }
}

//----------------------------------------------------------------------------------------------------------------------

class NullReport : public Report {
public:
    NullReport() {}
    ~NullReport() override {}
    virtual void json(JSON&) const override {}
};

class WraperReport : public Report {
public:
    WraperReport(std::function<void(JSON&)>& f) : callable_(f) {}
    ~WraperReport() override {}
    virtual void json(JSON& j) const override { callable_(j); }
    std::function<void(JSON&)>& callable_;
};

//----------------------------------------------------------------------------------------------------------------------

void Telemetry::report(Report::Type t) {
    NullReport r;
    report(t, r);
}

void Telemetry::report(Report::Type t, const Report& r) {
    Reporter::instance().report(t, r);
}

void Telemetry::report(Report::Type t, std::function<void(JSON&)> callable) {
    WraperReport r(callable);
    Reporter::instance().report(t, r);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace runtime
}  // namespace eckit
