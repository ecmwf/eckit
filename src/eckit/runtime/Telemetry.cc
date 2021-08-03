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
#include <string>
#include <vector>

#include "eckit/config/LibEcKit.h"
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
#include "eckit/utils/Clock.h"

namespace eckit {
namespace runtime {

//----------------------------------------------------------------------------------------------------------------------

Report::~Report() {}

JSON& operator<<(eckit::JSON& s, const Report& p) {
    p.json(s);
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

std::string report_type_to_name(Report::Type t) {

    ASSERT(t < Report::ENDTAG);  // ensure client code sticks to enum Report

    static const std::map<int, std::string> type_to_name = {
        {Report::APPSTART, "appstart"},
        {Report::APPSTOP, "appstop"},
        {Report::INFO, "info"},
        {Report::METER, "meter"},
        {Report::COUNTER, "counter"},
        {Report::KEEPALIVE, "keepalive"},
    };

    size_t count = type_to_name.size();

    ASSERT(count == Report::ENDTAG);  // ensure we didnt forget to populate the map

    auto r = type_to_name.find(t);
    if (r != type_to_name.end()) {
        return r->second;
    }

    NOTIMP;  // we shoudn't ever reach this
}

//----------------------------------------------------------------------------------------------------------------------

class Reporter : public NonCopyable {
public:
    static Reporter& instance();
    static int version() { return 1; }

    Reporter();

    ~Reporter() = default;

    bool enabled() { return not clients_.empty(); }

    void broadcast(const void* buf, size_t length);

    std::string report(Report::Type type, const Report& p);

    unsigned long long countSent() { return countSent_; }

private:
    std::vector<std::unique_ptr<net::UDPClient>> clients_;  //< list of clients to contact servers

    std::string service_type_;
    std::string service_name_;
    std::string node_name_;

    std::vector<std::string> service_groups_;

    unsigned long long countSent_ = 0;
};

//----------------------------------------------------------------------------------------------------------------------

Reporter& Reporter::instance() {
    static ThreadSingleton<Reporter> s;
    return s.instance();
}

static YAMLConfiguration* loadConfig() {

    char* config_str = ::getenv("TELEMETRY_CONFIG");
    if (config_str) {
        std::string s(config_str);
        return new YAMLConfiguration(s);
    }

    PathName telemetryConfigFile = Resource<PathName>("$TELEMETRY_CONFIG_FILE,telemetryConfigFile", "~/etc/config/telemetry.yaml");

    if (telemetryConfigFile.exists())
        return new YAMLConfiguration(telemetryConfigFile);

    return nullptr;
}

Reporter::Reporter() {

    std::unique_ptr<YAMLConfiguration> config(loadConfig());

    if (not config)
        return;

    LOG_DEBUG_LIB(LibEcKit) << "Telemetry config: " << *config << std::endl;

    service_type_   = config->getString("service_type", "unknown");
    service_name_   = config->getString("service_name", "unknown");
    service_groups_ = config->getStringVector("service_groups", {});

    node_name_ = Resource<std::string>("node", "unknown");  // same as in NodeInfo

    for (auto& cfg : config->getSubConfigurations("servers")) {
        clients_.emplace_back(new net::UDPClient(cfg));
    }
}

std::string Reporter::report(Report::Type type, const Report& report) {

    if (not enabled())  //< early return if we don't have servers configured
        return std::string();

    std::ostringstream out;
    JSON j(out);
    j.startObject();


    unsigned long thread = reinterpret_cast<unsigned long>(::pthread_self());

    j << "version" << version();
    j << "type" << report_type_to_name(type);
    j << "service_type" << service_type_;
    j << "service_name" << service_name_;
    j << "service_groups" << service_groups_;
    j << "node" << node_name_;
    j << "application" << Main::instance().name();
    j << "hostname" << Main::hostname();
    j << "pid" << int(::getpid());
    j << "thread" << thread;
    j << "time" << Clock::now();

    j << "report";
    {
        j.startObject();
        j << report;
        j.endObject();
    }

    j.endObject();

    std::string msg = out.str();

    LOG_DEBUG_LIB(LibEcKit) << "Telemetry message: " << msg << std::endl;

    broadcast(msg.data(), msg.size());

    return msg;
}

void Reporter::broadcast(const void* buf, size_t length) {
    for (auto& c : clients_) {
        c->send(buf, length);
    }
    countSent_++;
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
    WraperReport(std::function<void(JSON&)>& f) :
        callable_(f) {}
    ~WraperReport() override {}
    virtual void json(JSON& j) const override { callable_(j); }
    std::function<void(JSON&)>& callable_;
};

//----------------------------------------------------------------------------------------------------------------------

std::string Telemetry::report(Report::Type t) {
    NullReport r;
    return report(t, r);
}

std::string Telemetry::report(Report::Type t, const Report& r) {
    return Reporter::instance().report(t, r);
}

std::string Telemetry::report(Report::Type t, std::function<void(JSON&)> callable) {
    WraperReport r(callable);
    return Reporter::instance().report(t, r);
}

unsigned long long Telemetry::countSent() {
    return Reporter::instance().countSent();
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace runtime
}  // namespace eckit
