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

namespace {

StaticMutex local_mutex;

std::string iso(time_t t) {
    char buf[80];
    ::strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&t));
    return std::string(buf);
}

void add(Value& out, const std::vector<std::string>& path, size_t n, const Value& value) {

    size_t size = path.size();
    if (n + 1 == size) {
        out[path[n]] = value;
        return;
    }

    if (!out.contains(path[n])) {
        out[path[n]] = ValueMap{};
    }

    add(out[path[n]], path, n + 1, value);
}

Value cleanPaths(const Value& in) {

    if (in.isList()) {
        Value out             = Value::makeList();
        const ValueList& list = in;
        for (const auto& v : list) {
            out.append(cleanPaths(v));
        }
        return out;
    }
    else if (in.isMap()) {
        Value out = Value::makeMap();
        std::vector<std::string> path;
        Tokenizer parse(".");
        const ValueMap& map = in;
        for (const auto& [k, v] : map) {
            path.clear();
            parse(k, path);
            add(out, path, 0, v);
        }
        return out;
    }
    else {
        return in;
    }
}

std::string strLen(size_t n, const std::string& str) {
    std::ostringstream os;
    for (size_t i = str.length(); i < n; ++i)
        os << " ";
    os << str;
    return os.str();
}

//----------------------------------------------------------------------------------------------------------------------

struct MetricsPrefixInfo {
    std::string name;
    std::string groupFullName;
    std::set<std::string> groupKeys;
    Value groupValues;
    Value groupNextValue;

    MetricsPrefixInfo(const std::string& n) :
        name(n),
        groupFullName(""),
        groupKeys({}),
        groupValues(Value::makeList()),
        groupNextValue(Value::makeOrderedMap()) {}
    bool group() const { return !groupFullName.empty(); }
};
}  // namespace

//----------------------------------------------------------------------------------------------------------------------

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
    void pushGroup(const std::string&);
    void item();
    void pop();

    bool group() const;
    const std::string& groupName() const;

private:  // members

    std::map<std::string, time_t> timestamps_;
    std::set<std::string> keys_;
    std::vector<MetricsPrefixInfo> stack_;
    size_t stackIndex_{0};

    time_t created_;
    Value metrics_;

private:  // methods

    std::string full(const std::string& name) const;

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const MetricsCollector& m) {
        m.print(s);
        return s;
    }
};

static MetricsCollector* current_ = nullptr;

MetricsCollector::MetricsCollector() : created_(::time(nullptr)), metrics_(Value::makeOrderedMap()) {
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

std::string MetricsCollector::full(const std::string& name) const {
    std::ostringstream oss;
    const char* sep = "";
    for (size_t i = stackIndex_; i < stack_.size(); ++i) {
        oss << sep << stack_[i].name;
        sep = ".";
    }
    oss << sep << name;
    return oss.str();
}

void MetricsCollector::set(const std::string& name, const Value& value, bool overrideOk) {

    const std::string ff = full(name);

    if (group()) {
        auto& s = stack_[stackIndex_ - 1];
        if (!overrideOk) {
            if (s.groupKeys.find(ff) != s.groupKeys.end()) {
                Log::warning() << "MetricsCollector::set(" << ff << ") duplicate key inside group \"" << s.groupFullName
                               << "\", new=" << value << ", old=" << s.groupNextValue[ff] << std::endl;
            }
        }
        s.groupKeys.insert(ff);
        s.groupNextValue[ff] = value;
    }
    else {
        if (!overrideOk) {
            if (keys_.find(ff) != keys_.end()) {
                Log::warning() << "MetricsCollector::set(" << ff << ") duplicate key, new=" << value
                               << ", old=" << metrics_[ff] << std::endl;
            }
        }
        keys_.insert(ff);
        metrics_[ff] = value;
    }
}

void MetricsCollector::push(const std::string& name) {
    stack_.emplace_back(name);
}

void MetricsCollector::item() {
    if (!stack_.empty() && stackIndex_ > 0) {
        auto& ss = stack_[stackIndex_ - 1];

        if (ss.groupNextValue != Value::makeOrderedMap()) {
            ss.groupValues.append(cleanPaths(ss.groupNextValue));
            // ss.groupValues.append(ss.groupNextValue);
            ss.groupKeys.clear();
            ss.groupNextValue = Value::makeOrderedMap();
        }
    }
}

void MetricsCollector::pop() {
    if (stack_.empty())
        return;

    auto ss = stack_.back();
    if (ss.group()) {  // terminating a group
        --stackIndex_;
        for (size_t i = stackIndex_; i > 0 && !stack_[i - 1].group(); --i) {
            --stackIndex_;
        }

        if (stackIndex_ > 0) {  // it was a nested group
            auto& newss = stack_[stackIndex_ - 1];

            Value top             = Value::makeOrderedMap();
            top[ss.groupFullName] = ss.groupValues;
            newss.groupNextValue  = cleanPaths(top);
        }
        else {  // top level group - update metrics
            if (!metrics_.contains(ss.groupFullName)) {
                metrics_[ss.groupFullName] = Value::makeList();
            }
            ValueList ll = ss.groupValues;
            for (const auto& v : ll) {
                metrics_[ss.groupFullName].append(v);
            }
        }
    }
    stack_.pop_back();
}

void MetricsCollector::pushGroup(const std::string& name) {
    std::string groupFullName = full(name);
    if (stackIndex_ == 0) {
        // First group - if missing, create a placeholder in metrics
        if (!metrics_.contains(groupFullName)) {
            metrics_[groupFullName] = Value::makeList();
        }
        keys_.insert(groupFullName);
    }
    stack_.emplace_back(name);
    stack_.back().groupFullName = groupFullName;
    stackIndex_                 = stack_.size();
}

bool MetricsCollector::group() const {
    if (stack_.empty())
        return false;
    return stackIndex_ > 0;
}

const std::string& MetricsCollector::groupName() const {
    ASSERT(stackIndex_ > 0);
    return stack_[stackIndex_ - 1].name;
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

void MetricsCollector::set(const std::string& name, const std::map<std::string, unsigned long long>& value,
                           bool overrideOk) {
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

void MetricsCollector::print(std::ostream& s) const {
    JSON json(s);
    time_t now = ::time(nullptr);

    Value top = Value::makeOrderedMap();

    top["process"]    = eckit::Main::instance().name();
    top["start_time"] = iso(created_);
    top["end_time"]   = iso(now);
    top["run_time"]   = (now - created_);

    auto j = timestamps_.find("received");
    if (j != timestamps_.end()) {
        top["queue_time"] = (created_ - (*j).second);
    }

    Value out       = cleanPaths(metrics_);
    ValueMap outMap = out;
    for (const auto& [k, v] : outMap) {
        top[k] = v;
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

MetricsGroup::MetricsGroup(const std::string& prefix) {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->pushGroup(prefix);
    }
}

MetricsGroup::~MetricsGroup() {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->item();
        current_->pop();
    }
}

//----------------------------------------------------------------------------------------------------------------------

MetricsGroupItem::MetricsGroupItem() {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->item();
    }
}
MetricsGroupItem::~MetricsGroupItem() {
    AutoLock<StaticMutex> lock(local_mutex);
    if (current_) {
        current_->item();
    }
}

//----------------------------------------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------------------------------------

CollectMetrics::CollectMetrics() : collector_(new MetricsCollector()) {}

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
