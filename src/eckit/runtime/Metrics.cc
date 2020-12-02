#include "eckit/runtime/Metrics.h"

#include <ctime>
#include <set>
#include <string>
#include <vector>
#include "eckit/exception/Exceptions.h"
#include "eckit/log/JSON.h"
#include "eckit/runtime/Main.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"
#include "eckit/types/Types.h"


#include "eckit/memory/NonCopyable.h"
#include "eckit/value/Value.h"

namespace eckit {

class MetricsCollector;

static StaticMutex local_mutex;
static MetricsCollector* current_ = nullptr;

static std::string iso(time_t t) {
    char buf[80];
    ::strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&t));
    return std::string(buf);
}


class MetricsCollector : private eckit::NonCopyable {
public:  // methods
    MetricsCollector();
    ~MetricsCollector();

    void set(const std::string& name, const Value& value);
    void set(const std::string& name, const Value& value, const std::string& prefix);

    void set(const std::string& name, const std::vector<std::string>& value);
    void set(const std::string& name, const std::set<std::string>& value);

    void set(const std::string& name, const std::vector<std::string>& value, const std::string& prefix);
    void set(const std::string& name, const std::set<std::string>& value, const std::string& prefix);

    void set(const std::string& name, const std::map<std::string, unsigned long long>& value,
             const std::string& prefix);

    void timestamp(const std::string& name, time_t value);

    void error(const std::exception&);

    void send(Stream&) const;
    void receive(Stream&);
    void push(const std::string&);
    void pop(const std::string&);


private:
    void _pop(const std::string&);

private:  // members
    std::map<std::string, time_t> timestamps_;
    std::set<std::string> keys_;
    std::vector<MetricsCollector*> stack_;

    time_t created_;
    Value metrics_;

    MetricsCollector* top_;

private:  // methods
    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const MetricsCollector& m) {
        m.print(s);
        return s;
    }
};

MetricsCollector::MetricsCollector() : metrics_(Value::makeOrderedMap()), created_(::time(nullptr)), top_(nullptr) {
    AutoLock<StaticMutex> lock(local_mutex);
    top_     = current_;
    current_ = this;
}


MetricsCollector::~MetricsCollector() {
    AutoLock<StaticMutex> lock(local_mutex);
    while (!stack_.empty()) {
        delete stack_.back();
        stack_.pop_back();
    }
    ASSERT(current_ == this);
    current_ = top_;
}

void MetricsCollector::error(const std::exception& e) {
    set("error", e.what());
}

void MetricsCollector::set(const std::string& name, const Value& value) {
    if (keys_.find(name) != keys_.end()) {
        std::stringstream oss;
        oss << "MetricsCollector::set(" << name << ") duplicate key, new=" << value << ", old=" << metrics_[name];
        throw SeriousBug(oss.str());
    }
    keys_.insert(name);
    metrics_[name] = value;
}

void MetricsCollector::set(const std::string& name, const Value& value, const std::string& prefix) {

    if (!metrics_.contains(prefix)) {
        metrics_[prefix] = Value::makeOrderedMap();
    }

    std::string key = prefix + "." + name;
    ASSERT(keys_.find(key) == keys_.end());

    if (keys_.find(key) != keys_.end()) {
        std::stringstream oss;
        oss << "MetricsCollector::set(" << key << ") duplicate key, new=" << value << ", old=" << metrics_[key];
        throw SeriousBug(oss.str());
    }


    keys_.insert(key);

    metrics_[prefix][name] = value;
}

void MetricsCollector::push(const std::string& name) {
    stack_.push_back(new MetricsCollector());
}

void MetricsCollector::_pop(const std::string& prefix) {
    ASSERT(!stack_.empty());
    if (!metrics_.contains(prefix)) {
        metrics_[prefix] = Value::makeList();
    }

    MetricsCollector* last = stack_.back();
    stack_.pop_back();

    metrics_[prefix].append(last->metrics_);

    delete last;
}

void MetricsCollector::pop(const std::string& name) {
    ASSERT(top_);
    top_->_pop(name);
}


void MetricsCollector::timestamp(const std::string& name, time_t time) {
    timestamps_[name] = time;
    set(name, time);
}

void MetricsCollector::set(const std::string& name, const std::set<std::string>& value, const std::string& prefix) {
    set(name, toValue(value), prefix);
}

void MetricsCollector::set(const std::string& name, const std::vector<std::string>& value, const std::string& prefix) {
    set(name, toValue(value), prefix);
}

void MetricsCollector::set(const std::string& name, const std::map<std::string, unsigned long long>& value,
                           const std::string& prefix) {
    set(name, toValue(value), prefix);
}

void MetricsCollector::send(Stream& s) const {
    s << metrics_;
}

void MetricsCollector::receive(Stream& s) {
    Value v(s);
    ValueMap m = v;
    for (auto j = m.begin(); j != m.end(); ++j) {
        metrics_[(*j).first] = (*j).second;
    }
}

void MetricsCollector::print(std::ostream& s) const {
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

AutoPushingMetrics::AutoPushingMetrics(const std::string& prefix) : prefix_(prefix) {
    ASSERT(prefix_.size());
    Metrics::push(prefix_);
}

AutoPushingMetrics::~AutoPushingMetrics() {
    Metrics::pop(prefix_);
}

//=============================================================================


void Metrics::set(const std::string& name, const std::map<std::string, unsigned long long>& value,
                  const std::string& prefix) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->set(name, value, prefix);
    }
}

void Metrics::set(const std::string& name, const std::set<std::string>& value, const std::string& prefix) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->set(name, value, prefix);
    }
}

void Metrics::set(const std::string& name, const std::vector<std::string>& value, const std::string& prefix) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->set(name, value, prefix);
    }
}


void Metrics::set(const std::string& name, const eckit::Value& value, const std::string& prefix) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->set(name, value, prefix);
    }
}

void Metrics::set(const std::string& name, const eckit::Value& value) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->set(name, value);
    }
}

void Metrics::receive(eckit::Stream& s) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->receive(s);
    }
}

void Metrics::send(eckit::Stream& s) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->send(s);
    }
}


void Metrics::push(const std::string& prefix) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->push(prefix);
    }
}

void Metrics::pop(const std::string& prefix) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->pop(prefix);
    }
}

void Metrics::error(const std::exception& message) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->error(message);
    }
}


void Metrics::timestamp(const std::string& name, time_t time) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->timestamp(name, time);
    }
}

AutoCollectMetrics::AutoCollectMetrics() : collector_(new MetricsCollector()) {}

AutoCollectMetrics::~AutoCollectMetrics() {
    delete collector_;
}

void AutoCollectMetrics::print(std::ostream& s) const {
    s << *collector_;
}


}  // namespace eckit
