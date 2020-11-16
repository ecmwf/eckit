#include "eckit/runtime/Metrics.h"

#include <ctime>

#include "eckit/log/JSON.h"
#include "eckit/runtime/Main.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"

namespace eckit {

static StaticMutex local_mutex;
static Metrics* current_ = nullptr;

class Metric {
protected:
    std::string name_;

public:
    virtual ~Metric() = default;
    Metric(const std::string& name) : name_(name) {}

    virtual void json(JSON& s) const   = 0;
    virtual void send(Stream& s) const = 0;

    const std::string& name() const { return name_; }
};


class DoubleMetric : public Metric {
    double value_;

public:
    DoubleMetric(const std::string& name, double value) : Metric(name), value_(value){};
    virtual void json(JSON& s) const { s << name_ << value_; }
    virtual void send(Stream& s) const { s << 'd' << name_ << value_; }
};

class IntegerMetric : public Metric {
    long long value_;

public:
    IntegerMetric(const std::string& name, long long value) : Metric(name), value_(value){};
    virtual void json(JSON& s) const { s << name_ << value_; }
    virtual void send(Stream& s) const { s << 'i' << name_ << value_; }
};

class StringMetric : public Metric {
    std::string value_;

public:
    StringMetric(const std::string& name, const std::string& value) : Metric(name), value_(value){};
    virtual void json(JSON& s) const { s << name_ << value_; }
    virtual void send(Stream& s) const { s << 's' << name_ << value_; }
};

class StringVectorMetric : public Metric {
    std::vector<std::string> value_;

public:
    StringVectorMetric(const std::string& name, const std::vector<std::string>& value) : Metric(name), value_(value){};
    virtual void json(JSON& s) const { s << name_ << value_; }
    virtual void send(Stream& s) const { s << 'S' << name_ << value_; }
};


Metrics::Metrics(bool print) : printed_(!print) {
    AutoLock<StaticMutex> lock(local_mutex);
    ASSERT(current_ == nullptr);
    current_ = this;
    set("process", eckit::Main::instance().name());
}

Metrics::~Metrics() {
    AutoLock<StaticMutex> lock(local_mutex);
    ASSERT(current_ == this);
    current_ = nullptr;
    if (!printed_) {
        eckit::Log::metrics() << *this << std::endl;
    }

    for (auto m : metrics_) {
        delete m;
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
    metrics_.push_back(new StringMetric(name, value));
}

void Metrics::set(const std::string& name, const std::string& value) {
    metrics_.push_back(new StringMetric(name, value));
}

void Metrics::set(const std::string& name, unsigned long value) {
    metrics_.push_back(new IntegerMetric(name, value));
}

void Metrics::set(const std::string& name, unsigned long long value) {
    metrics_.push_back(new IntegerMetric(name, value));
}

void Metrics::set(const std::string& name, long long value) {
    metrics_.push_back(new IntegerMetric(name, value));
}

void Metrics::set(const std::string& name, long value) {
    metrics_.push_back(new IntegerMetric(name, value));
}

void Metrics::set(const std::string& name, int value) {
    metrics_.push_back(new IntegerMetric(name, value));
}

void Metrics::set(const std::string& name, unsigned int value) {
    metrics_.push_back(new IntegerMetric(name, value));
}


void Metrics::set(const std::string& name, double value) {
    metrics_.push_back(new DoubleMetric(name, value));
}

void Metrics::timestamp(const std::string& name, time_t value) {
    char buf[80];
    ::strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&value));
    set(name, buf);
}

void Metrics::set(const std::string& name, const std::vector<std::string>& value) {
    metrics_.push_back(new StringVectorMetric(name, value));
}

void Metrics::set(const std::string& name, const std::set<std::string>& value) {
    std::vector<std::string> v(value.begin(), value.end());
    set(name, v);
}


void Metrics::print(std::ostream& s) const {
    JSON json(s);
    json.startObject();
    for (auto m : metrics_) {
        m->json(json);
    }
    json.endObject();
    printed_ = true;
}

void Metrics::send(Stream& s) const {

    bool first = true;
    s << metrics_.size() - 1;
    for (auto m : metrics_) {
        if (!first) {  // skip remote "process"
            m->send(s);
        }
        first = false;
    }
}

void Metrics::receive(Stream& s) {

    size_t cnt;
    s >> cnt;

    for (size_t i = 0; i < cnt; ++i) {
        char op;
        std::string name;
        s >> op;
        s >> name;

        long long intval;
        double dblval;
        std::string strval;
        std::vector<std::string> svecval;

        switch (op) {
            case 'i':
                s >> intval;
                metrics_.push_back(new IntegerMetric(name, intval));
                break;

            case 'd':
                s >> dblval;
                metrics_.push_back(new DoubleMetric(name, dblval));
                break;

            case 's':
                s >> strval;
                metrics_.push_back(new StringMetric(name, strval));
                break;

            case 'S':
                s >> svecval;
                metrics_.push_back(new StringVectorMetric(name, svecval));
                break;

            default:
                throw SeriousBug(std::string("Invalid metric tag: ") + op);
                break;
        }
    }
}

}  // namespace eckit
