#include "eckit/runtime/Metrics.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/StaticMutex.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {

static StaticMutex local_mutex;
static Metrics* current_ = nullptr;

Metrics::Metrics() : metrics_(eckit::Value::makeOrderedMap()) {
    AutoLock<StaticMutex> lock(local_mutex);
    ASSERT(current_ == nullptr);
    current_ = this;
    set("process", eckit::Main::instance().name());
}

Metrics::~Metrics() {
    AutoLock<StaticMutex> lock(local_mutex);
    ASSERT(current_ == this);
    current_ = nullptr;
    if(!printed_) {
        eckit::Log::metrics() << *this << std::endl;
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

void Metrics::set(const std::string& name, const char* value) {
    metrics_[name] = value;
}

void Metrics::set(const std::string& name, const std::string& value) {
    metrics_[name] = value;
}

void Metrics::set(const std::string& name, unsigned long value) {
    metrics_[name] = value;
}

void Metrics::set(const std::string& name, unsigned long long value) {
    metrics_[name] = value;
}

void Metrics::set(const std::string& name, long long value) {
    metrics_[name] = value;
}

void Metrics::set(const std::string& name, long value) {
    metrics_[name] = value;
}

void Metrics::set(const std::string& name, int value) {
    metrics_[name] = value;
}

void Metrics::set(const std::string& name, unsigned int value) {
    metrics_[name] = value;
}


void Metrics::set(const std::string& name, double value) {
    metrics_[name] = value;
}

// void Metrics::set(const std::string& name, const std::vector<std::string>& value) {
//     metrics_[name] = eckit::Value::makeList(value);
// }

void Metrics::print(std::ostream& s) const {
    eckit::ValueList keys = metrics_.keys();
    const char* sep       = "";
    for (auto key : keys) {
        eckit::Value value = metrics_[key];
        if (value.isList()) {
            s << sep << key << "=" << value;
        }
        else {
            s << sep << key << "=" << value;
        }
        sep = " ";
    }
    printed_ = true;
}

void Metrics::send(Stream& s) const {
    s << metrics_;
}

void Metrics::receive(Stream& s) {
    Value value(s);
    ValueMap m(value);
    Value process("process");
    for (auto j = m.begin(); j != m.end(); ++j) {
        if((*j).first != process) {
            metrics_[(*j).first] = (*j).second;
        }
    }

}


}  // namespace eckit
