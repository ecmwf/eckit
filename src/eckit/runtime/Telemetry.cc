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
    [Report::METER]    = "meter",
};

const char* report_type_to_name(Report::Type t) {
    if (t < Report::MAXREPORT)
        return report_type_name[t];
    /* otherwise */
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

class Reporter : public NonCopyable {
public:  // methods
    static Reporter& instance();
    static int version() { return 1; }

    Reporter();

    ~Reporter() = default;

    bool enabled() { return not clients_.empty(); }

    void broadcast(void* buf, long length);

    void report(Report::Type type, const Report& p);

    void header(Report::Type type, JSON& j);

private:                                                    // members
    std::vector<std::unique_ptr<net::UDPClient>> clients_;  //< list of clients to contact servers

    std::string service_type;
    std::string service_name;
};

//----------------------------------------------------------------------------------------------------------------------

Reporter& Reporter::instance() {
    static ThreadSingleton<Reporter> s;
    return s.instance();
}

Reporter::Reporter() {

    ECKIT_DEBUG_VAR(sizeof(report_type_name));

    PathName path = "~/etc/config/telemetry.yaml";

    if (not path.exists())
        return;

    YAMLConfiguration config(path);

    ECKIT_DEBUG_VAR(config)

    service_type = config.getString("service_type", "unknown");
    service_name = config.getString("service_name", "unknown");

    for (auto& cfg : config.getSubConfigurations("servers")) {
        clients_.emplace_back(new net::UDPClient(cfg));
    }
}

void Reporter::header(Report::Type type, JSON& j) {
    j << "header";
    j.startObject();
    j << "version" << version();
    j << "report" << report_type_to_name(type);
    j << "service_type" << service_type;
    j << "service_name" << service_name;
    j << "application" << Main::instance().name();
    j << "hostname" << Main::hostname();
    j << "pid" << ::getpid();
    j << "thread" << ::pthread_self();
    j << "time" << ::time(nullptr);
    j.endObject();
}

void Reporter::report(Report::Type type, const Report& report) {

    std::ostringstream out;
    JSON j(out);
    j.startObject();
    header(type, j);
    j << report;
    j.endObject();

    std::string str = out.str();

    ECKIT_DEBUG_VAR(str)

    broadcast((void*)str.data(), str.size());
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

//----------------------------------------------------------------------------------------------------------------------

void Telemetry::report(Report::Type t) {
    NullReport r;
    report(t, r);
}

void Telemetry::report(Report::Type t, Report& r) {
    if (not Reporter::instance().enabled())
        return;
    Reporter::instance().report(t, r);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace runtime
}  // namespace eckit
