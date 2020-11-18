#include "eckit/runtime/Metrics.h"

#include <ctime>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/JSON.h"
#include "eckit/runtime/Main.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"
#include "eckit/types/Types.h"

namespace eckit {

static StaticMutex local_mutex;
static Metrics* current_ = nullptr;

static std::string iso(time_t t) {
    char buf[80];
    ::strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&t));
    return std::string(buf);
}


Metrics::Metrics() : metrics_(Value::makeOrderedMap()), created_(::time(nullptr)) {
    AutoLock<StaticMutex> lock(local_mutex);
    ASSERT(current_ == nullptr);
    current_ = this;
}


Metrics::~Metrics() {

    AutoLock<StaticMutex> lock(local_mutex);
    if (current_ == this) {
        current_ = nullptr;
    }
}

Metrics& Metrics::current() {
    AutoLock<StaticMutex> lock(local_mutex);
    ASSERT(current_);
    return *current_;
}

void Metrics::error(const std::exception& e) {
    set("error", e.what());
}

void Metrics::set(const std::string& name, const Value& value) {
    metrics_[name] = value;
}

void Metrics::set(const std::string& name, const Value& value, const std::string& prefix) {

    if (!metrics_.contains(prefix)) {
        metrics_[prefix] = Value::makeOrderedMap();
    }

    metrics_[prefix][name] = value;
}

void Metrics::timestamp(const std::string& name, time_t time) {
    timestamps_[name] = time;
    set(name, time);
}

void Metrics::set(const std::string& name, const std::set<std::string>& value, const std::string& prefix) {
    set(name, toValue(value), prefix);
}

void Metrics::set(const std::string& name, const std::vector<std::string>& value, const std::string& prefix) {
    set(name, toValue(value), prefix);
}

void Metrics::send(Stream& s) const {
    s << metrics_;
}

void Metrics::receive(Stream& s) {
    Value v(s);
    ValueMap m = v;
    for (auto j = m.begin(); j != m.end(); ++j) {
        metrics_[(*j).first] = (*j).second;
    }
}

void Metrics::print(std::ostream& s) const {
    JSON json(s);
    time_t now = ::time(0);

    json.startObject();


    json << "process" << eckit::Main::instance().name();
    json << "start_time" << iso(created_);
    json << "end_time" << iso(now);
    json << "run_time" << (now - created_);

    auto j = timestamps_.find("received");
    if (j != timestamps_.end()) {
        json << "queue_time" << (created_ - (*j).second);
    }


    ValueList keys = metrics_.keys();
    for (std::string key : keys) {
        json << key;
        auto j = timestamps_.find(key);
        if (j != timestamps_.end()) {
            json << iso(metrics_[key]);
        }
        else {
            json << metrics_[key];
        }
    }

    json.endObject();
}


}  // namespace eckit
