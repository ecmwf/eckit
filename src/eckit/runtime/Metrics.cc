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

#include "eckit/utils/Tokenizer.h"

#include "eckit/memory/NonCopyable.h"
#include "eckit/value/Value.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

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

    void set(const std::string& name, const Value& value, bool overrideOk);

    void set(const std::string& name, const std::set<std::string>& value, bool overrideOk);

    void set(const std::string& name, const std::vector<std::string>& value, bool overrideOk);

    void set(const std::string& name, const std::map<std::string, unsigned long long>& value, bool overrideOk);

    void timestamp(const std::string& name, time_t value, bool overrideOk);

    void error(const std::exception&);

    void send(Stream&) const;
    void receive(Stream&);
    void push(const std::string&);
    void pop();


private:  // members
    std::map<std::string, time_t> timestamps_;
    std::set<std::string> keys_;
    std::vector<std::string> stack_;

    time_t created_;
    Value metrics_;

private:  // methods
    void print(std::ostream&) const;

    void add(Value& top, const std::vector<std::string>& v, size_t n, const Value& value) const;


    friend std::ostream& operator<<(std::ostream& s, const MetricsCollector& m) {
        m.print(s);
        return s;
    }
};

MetricsCollector::MetricsCollector() :
    created_(::time(nullptr)), metrics_(Value::makeOrderedMap()) {
    AutoLock<StaticMutex> lock(local_mutex);
    ASSERT(current_ == nullptr);
    current_ = this;
}


MetricsCollector::~MetricsCollector() {
    AutoLock<StaticMutex> lock(local_mutex);
    ASSERT(current_ == this);
    current_ = nullptr;
}

void MetricsCollector::error(const std::exception& e) {
    set("error", e.what(), true);
}

void MetricsCollector::set(const std::string& name, const Value& value, bool overrideOk) {

    std::stringstream oss;
    const char* sep = "";
    for (const std::string& s : stack_) {
        oss << sep << s;
        sep = ".";
    }
    oss << sep << name;
    std::string full = oss.str();

    if (!overrideOk) {
        if (keys_.find(full) != keys_.end()) {
            // std::stringstream oss;
            Log::warning() << "MetricsCollector::set(" << full << ") duplicate key, new=" << value << ", old=" << metrics_[full] << std::endl;
            // throw SeriousBug(oss.str());
        }
    }

    keys_.insert(full);
    metrics_[full] = value;
}

void MetricsCollector::push(const std::string& name) {
    stack_.push_back(name);
}


void MetricsCollector::pop() {
    stack_.pop_back();
}


void MetricsCollector::timestamp(const std::string& name, time_t time, bool overrideOk) {
    timestamps_[name] = time;
    set(name, iso(time), overrideOk);
}

void MetricsCollector::set(const std::string& name, const std::vector<std::string>& value, bool overrideOk) {
    set(name, toValue(value), overrideOk);
}

void MetricsCollector::set(const std::string& name, const std::set<std::string>& value, bool overrideOk) {
    set(name, toValue(value), overrideOk);
}

void MetricsCollector::set(const std::string& name, const std::map<std::string, unsigned long long>& value, bool overrideOk) {
    set(name, toValue(value), overrideOk);
}

void MetricsCollector::send(Stream& s) const {
    s << metrics_;
}

void MetricsCollector::receive(Stream& s) {
    Value v(s);
    ValueMap m = v;
    for (auto j = m.begin(); j != m.end(); ++j) {
        std::string key = (*j).first;
        set(key, (*j).second, false);
    }
}

void MetricsCollector::add(Value& top, const std::vector<std::string>& path, size_t n, const Value& value) const {


    size_t size = path.size();
    if (n + 1 == size) {
        top[path[n]] = value;
        return;
    }

    if (!top.contains(path[n])) {
        top[path[n]] = Value::makeOrderedMap();
    }

    add(top[path[n]], path, n + 1, value);
}

void MetricsCollector::print(std::ostream& s) const {
    JSON json(s);
    time_t now = ::time(0);

    Value top = Value::makeOrderedMap();

    top["process"]    = eckit::Main::instance().name();
    top["start_time"] = iso(created_);
    top["end_time"]   = iso(now);
    top["run_time"]   = (now - created_);

    auto j = timestamps_.find("received");
    if (j != timestamps_.end()) {
        top["queue_time"] = (created_ - (*j).second);
    }


    std::vector<std::string> path;
    ValueMap metrics = metrics_;
    Tokenizer parse(".");

    for (auto j = metrics.begin(); j != metrics.end(); ++j) {
        path.clear();
        parse((*j).first, path);
        add(top, path, 0, (*j).second);
    }

    json << top;
}

//----------------------------------------------------------------------------------------------------------------------

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

MetricsPrefix::MetricsPrefix(const std::string& prefix) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->push(prefix);
    }
}

MetricsPrefix::~MetricsPrefix() {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->pop();
    }
}

void Metrics::error(const std::exception& message) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->error(message);
    }
}

void Metrics::timestamp(const std::string& name, time_t time, bool overrideOk) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->timestamp(name, time, overrideOk);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Metrics::set(const std::string& name, const eckit::Value& value, bool overrideOk) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->set(name, value, overrideOk);
    }
}

void Metrics::set(const std::string& name, const std::vector<std::string>& value, bool overrideOk) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->set(name, value, overrideOk);
    }
}

void Metrics::set(const std::string& name, const std::set<std::string>& value, bool overrideOk) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->set(name, value, overrideOk);
    }
}

void Metrics::set(const std::string& name, const std::map<std::string, unsigned long long>& value, bool overrideOk) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->set(name, value, overrideOk);
    }
}

void Metrics::set(const std::string& name, bool value, bool overrideOk) {
    set(name, Value(value), overrideOk);
}

void Metrics::set(const std::string& name, int value, bool overrideOk) {
    set(name, Value(value), overrideOk);
}

void Metrics::set(const std::string& name, unsigned int value, bool overrideOk) {
    set(name, Value(value), overrideOk);
}

void Metrics::set(const std::string& name, long value, bool overrideOk) {
    set(name, Value(value), overrideOk);
}

void Metrics::set(const std::string& name, unsigned long value, bool overrideOk) {
    set(name, Value(value), overrideOk);
}

void Metrics::set(const std::string& name, long long value, bool overrideOk) {
    set(name, Value(value), overrideOk);
}

void Metrics::set(const std::string& name, unsigned long long value, bool overrideOk) {
    set(name, Value(value), overrideOk);
}

void Metrics::set(const std::string& name, double value, bool overrideOk) {
    set(name, Value(value), overrideOk);
}

void Metrics::set(const std::string& name, const Length& value, bool overrideOk) {
    set(name, static_cast<unsigned long long>(value), overrideOk);
}

void Metrics::set(const std::string& name, const Offset& value, bool overrideOk) {
    set(name, static_cast<long long>(value), overrideOk);
}

//----------------------------------------------------------------------------------------------------------------------

CollectMetrics::CollectMetrics() :
    collector_(new MetricsCollector()) {}

CollectMetrics::~CollectMetrics() {
    delete collector_;
}

void CollectMetrics::print(std::ostream& s) const {
    // Avoid that long lines are truncated and mixed up
    std::ostringstream oss;
    oss << *collector_;
    s << oss.str();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
