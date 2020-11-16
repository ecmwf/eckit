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


static std::string iso(time_t t) {
    char buf[80];
    ::strftime(buf, sizeof(buf), "%FT%TZ", gmtime(&t));
    return std::string(buf);
}


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

class TimeMetric : public Metric {
    time_t value_;

public:
    TimeMetric(const std::string& name, time_t value) : Metric(name), value_(value){};
    virtual void json(JSON& s) const {
        s << name_;
        s << iso(value_);
    }
    virtual void send(Stream& s) const { s << 't' << name_ << value_; }
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


Metrics::Metrics(bool main) : printed_(!main), created_(::time(nullptr)) {

    AutoLock<StaticMutex> lock(local_mutex);
    ASSERT(current_ == nullptr);
    current_ = this;
    if (main) {
        set("process", eckit::Main::instance().name());
    }
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
    metrics_.push_back(new TimeMetric(name, value));
}

void Metrics::set(const std::string& name, const std::vector<std::string>& value) {
    metrics_.push_back(new StringVectorMetric(name, value));
}

void Metrics::set(const std::string& name, const std::set<std::string>& value) {
    std::vector<std::string> v(value.begin(), value.end());
    set(name, v);
}

void Metrics::json(JSON& json) const {
    for (auto m : metrics_) {
        m->json(json);
    }
}

void Metrics::print(std::ostream& s) const {
    time_t now = time(nullptr);
    JSON json(s);
    json.startObject();
    json << "start_time" << iso(created_);
    json << "end_time" << iso(now);
    json << "duration" << (now - created_);
    for (auto m : metrics_) {
        m->json(json);
    }
    json.endObject();
    printed_ = true;
}

void Metrics::send(Stream& s) const {

    s << metrics_.size();
    for (auto m : metrics_) {
        m->send(s);
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

            case 't':
                s >> intval;
                metrics_.push_back(new TimeMetric(name, intval));
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
