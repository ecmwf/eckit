#include <memory>
#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Log.h"
#include "eckit/maths/Functions.h"
#include "eckit/mpi/Comm.h"
#include "eckit/mpi/Request.h"
#include "eckit/runtime/Tool.h"
#include "eckit/serialisation/ResizableMemoryStream.h"
#include "eckit/types/Types.h"

//----------------------------------------------------------------------------------------------------------------------

class SerializeObject {

protected:
    /// De-serialize vectors
    /// Do not use for large vectors / arrays
    template <typename T>
    void readVector(std::vector<T>& v, eckit::Stream& s) const {
        size_t sz;
        s >> sz;
        v.resize(sz);
        for (auto& value : v) {
            s >> value;
        }
    }

    /// Serialize vectors
    /// Do not use for large vectors / arrays
    template <typename T>
    void writeVector(const std::vector<T>& v, eckit::Stream& s) const {
        s << v.size();
        for (auto& value : v) {
            s << value;
        }
    }
};


class Obj : protected SerializeObject {
public:
    Obj(const std::string& s, int i, double d, bool b, size_t sz = 2) :
        s_(s), i_(i), d_(d), b_(b), data_(i * sz) {
        for (auto& value : data_) {
            value = 2 * i;
        }
    }

    /// Decodes an object from a stream
    Obj(eckit::Stream& s) {
        s >> s_;
        s >> i_;
        s >> d_;
        s >> b_;
        readVector(data_, s);
        bool next;
        s >> next;
        if (next) {
            next_.reset(new Obj(s));
        }
    }

    /// Encodes an object into a stream
    void encode(eckit::Stream& s) const {
        s << s_ << i_ << d_ << b_;
        writeVector(data_, s);  // do not use for large vectors / arrays
        if (next_) {
            s << true;
            next_->encode(s);
        }
        else {
            s << false;
        }
    }

    void add(Obj* o) {
        if (!next_) {
            next_.reset(o);
            return;
        }
        next_->add(o);
    }

    bool operator==(const Obj& rhs) {

        auto compvec = [&]() {
            for (size_t i = 0; i < data_.size(); ++i) {
                if (data_[i] != rhs.data_[i])
                    return false;
            }
            return true;
        };

        auto compnext = [&]() {
            if (not next_ && not rhs.next_) {
                return true;
            }
            if (next_ && rhs.next_) {
                return *next_ == *rhs.next_;
            }
            return false;
        };

        return s_ == rhs.s_ && i_ == rhs.i_ && d_ == rhs.d_ && b_ == rhs.b_ && data_.size() == rhs.data_.size() && compvec() && compnext();
    }

    void print(std::ostream& os) const {
        os << "Obj(" << s_ << "," << i_ << "," << d_ << "," << b_ << "," << data_;

        if (next_)
            os << "," << *next_;

        os << ")";
    }

    friend std::ostream& operator<<(std::ostream& os, const Obj& p) {
        p.print(os);
        return os;
    }

private:  // members
    std::string s_;
    int i_;
    double d_;
    bool b_;

    std::vector<double> data_;

    std::unique_ptr<Obj> next_;
};

//----------------------------------------------------------------------------------------------------------------------

size_t circlel(size_t i, size_t total) {
    return (i + 1) % total;
}

size_t circler(size_t i, size_t total) {
    if (i == 0)
        return total - 1;
    return (i - 1) % total;
}


//----------------------------------------------------------------------------------------------------------------------

static size_t me = 0;

class ObjSend : public eckit::Tool {
public:
    ObjSend(int argc, char** argv) :
        Tool(argc, argv, "HOME"), sendBuffer_(32) { sendBuffer_.zero(); }

    ~ObjSend() {}

private:
    eckit::Buffer sendBuffer_;

    ObjSend(const ObjSend&) = delete;
    ObjSend& operator=(const ObjSend&) = delete;

    void send(const Obj& o, size_t to) {

        eckit::Log::info() << "[" << me << "] "
                           << "sending to " << to << " --- " << o << std::endl;

        eckit::ResizableMemoryStream s(sendBuffer_);

        o.encode(s);

        eckit::Log::info() << "[" << me << "] "
                           << "stream position: " << s.position() << " / " << sendBuffer_.size() << std::endl;

        // use position in stream to avoid sending the whole buffer
        // assumes the receive probes for size first

        eckit::mpi::comm().iSend(static_cast<const char*>(sendBuffer_.data()), s.position(), int(to), 0);
    }

    Obj receive(size_t from) {

        eckit::mpi::Status st = eckit::mpi::comm().probe(int(from), 0);

        size_t size = eckit::round(eckit::mpi::comm().getCount<char>(st), 8);  //< round to nearest 8 bytes

        eckit::Log::info() << "[" << me << "] "
                           << "receiving from " << from << " --- size " << size << std::endl;

        eckit::Buffer b(size);  // must be enough
        b.zero();

        eckit::mpi::comm().receive(static_cast<char*>(b.data()), b.size(), int(from), 0);

        eckit::ResizableMemoryStream s(b);

        Obj o(s);

        eckit::Log::info() << "[" << me << "] "
                           << "receiving from " << from << " --- " << o << std::endl;

        return o;
    }

    virtual void run() {

        me = eckit::mpi::comm().rank();

        size_t total = eckit::mpi::comm().size();

        ASSERT_MSG(total > 1, "Must be ran with more than 1 rank");

        size_t to   = circlel(me, total);
        size_t from = circler(me, total);

        eckit::Log::info() << "[" << me << "] " << from << " -> [" << me << "] -> " << to << std::endl;

        //-----------------------------------------------
        // TEST Simple objects
        //-----------------------------------------------
        {
            Obj so{"foo", int(me), 374., true};
            send(so, to);

            Obj ro = receive(from);
            ASSERT(ro == Obj("foo", int(from), 374., true));
        }

        //-----------------------------------------------
        // TEST Complex object hierarchy
        //-----------------------------------------------
        {
            Obj so{"foo", int(me), 374., true};
            so.add(new Obj("foo", int(me), 374., true));
            so.add(new Obj("foo", int(me), 374., true));
            so.add(new Obj("foo", int(me), 374., true));

            send(so, to);

            Obj ro = receive(from);

            Obj test{"foo", int(from), 374., true};
            test.add(new Obj("foo", int(from), 374., true));
            test.add(new Obj("foo", int(from), 374., true));
            test.add(new Obj("foo", int(from), 374., true));

            ASSERT(ro == test);
        }
    }
};

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    ObjSend app(argc, argv);
    int ret = app.start();
    eckit::mpi::finaliseAllComms();
    return ret;
}
