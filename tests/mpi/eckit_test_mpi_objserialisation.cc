/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

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

struct Fixture {
public:
    Fixture() :
        sendBuffer_(32) {
        me_    = eckit::mpi::comm().rank();
        total_ = eckit::mpi::comm().size();

        ASSERT_MSG(total_ > 1, "Must be ran with more than 1 rank");

        sendBuffer_.zero();
    }

    size_t me_    = 0;
    size_t total_ = 0;

    eckit::Buffer sendBuffer_;

    Fixture(const Fixture&) = delete;
    Fixture& operator=(const Fixture&) = delete;

    void send(const Obj& o, size_t to) {

        eckit::Log::info() << "[" << me_ << "] "
                           << "sending to " << to << " --- " << o << std::endl;

        eckit::ResizableMemoryStream s(sendBuffer_);

        o.encode(s);

        eckit::Log::info() << "[" << me_ << "] "
                           << "stream position: " << s.position() << " / " << sendBuffer_.size() << std::endl;

        // use position in stream to avoid sending the whole buffer
        // assumes the receive probes for size first

        eckit::mpi::comm().iSend(static_cast<const char*>(sendBuffer_.data()), s.position(), int(to), 0);
    }

    Obj receive(size_t from) {

        eckit::mpi::Status st = eckit::mpi::comm().probe(int(from), 0);

        size_t size = eckit::round(eckit::mpi::comm().getCount<char>(st), 8);  //< round to nearest 8 bytes

        eckit::Log::info() << "[" << me_ << "] "
                           << "receiving from " << from << " --- size " << size << std::endl;

        eckit::Buffer b(size);  // must be enough
        b.zero();

        eckit::mpi::comm().receive(static_cast<char*>(b.data()), b.size(), int(from), 0);

        eckit::ResizableMemoryStream s(b);

        Obj o(s);

        eckit::Log::info() << "[" << me_ << "] "
                           << "receiving from " << from << " --- " << o << std::endl;

        return o;
    }
};

//----------------------------------------------------------------------------------------------------------------------

CASE("Serialise an object via eckit::mpi") {
    Fixture F;

    size_t me   = F.me_;
    size_t to   = circlel(F.me_, F.total_);
    size_t from = circler(F.me_, F.total_);

    eckit::Log::info() << "[" << me << "] " << from << " -> [" << me << "] -> " << to << std::endl;

    Obj so{"foo", int(me), 374., true};
    F.send(so, to);

    Obj ro = F.receive(from);

    EXPECT(ro == Obj("foo", int(from), 374., true));
}

CASE("Serialise a series of complex objects via eckit::mpi") {
    Fixture F;

    size_t me   = F.me_;
    size_t to   = circlel(F.me_, F.total_);
    size_t from = circler(F.me_, F.total_);

    eckit::Log::info() << "[" << me << "] " << from << " -> [" << me << "] -> " << to << std::endl;

    Obj so{"foo", int(me), 374., true};
    so.add(new Obj("foo", int(me), 374., true));
    so.add(new Obj("foo", int(me), 374., true));
    so.add(new Obj("foo", int(me), 374., true));

    F.send(so, to);

    Obj ro = F.receive(from);

    Obj test{"foo", int(from), 374., true};
    test.add(new Obj("foo", int(from), 374., true));
    test.add(new Obj("foo", int(from), 374., true));
    test.add(new Obj("foo", int(from), 374., true));

    EXPECT(ro == test);
}

}  // namespace test
}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
