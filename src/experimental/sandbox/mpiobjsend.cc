#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/runtime/Tool.h"
#include "eckit/serialisation/MemoryStream.h"
#include "eckit/mpi/Comm.h"
#include "eckit/mpi/Request.h"
#include "eckit/log/Log.h"
#include "eckit/types/Types.h"

//----------------------------------------------------------------------------------------------------------------------

class Obj {
public:

    Obj(eckit::Stream& s) {
        s >> s_;
        s >> i_;
        s >> d_;
        s >> b_;
    }

    Obj(const std::string& s, int i, double d, bool b) :
        s_(s),
        i_(i),
        d_(d),
        b_(b) {
    }

    void encode(eckit::Stream& s) const {
        s << s_
          << i_
          << d_
          << b_;
    }

    bool operator==(const Obj& rhs) {
        return s_ == rhs.s_ &&
               i_ == rhs.i_ &&
               d_ == rhs.d_ &&
               b_ == rhs.b_;
    }

    void print(std::ostream& os) const
    {
        os << "Obj("
           << s_ << ","
           << i_ << ","
           << d_ << ","
           << b_ << "]";
    }

    friend std::ostream& operator<<(std::ostream& os, const Obj& p) { p.print(os); return os; }

private: // members

    std::string s_;
    int i_;
    double d_;
    bool b_;

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

static const size_t BSIZE = 4*1024;

static size_t me = 0;

class ObjSend : public eckit::Tool {
public:

    ObjSend(int argc, char** argv) :
        Tool(argc, argv, "HOME"),
        sendBuffer_(BSIZE)
    {
        sendBuffer_.zero();
    }

    ~ObjSend() {}

private:

    eckit::Buffer sendBuffer_;

    ObjSend(const ObjSend&) = delete;
    ObjSend& operator=(const ObjSend&) = delete;

    void send(const Obj& o, size_t to) {

        eckit::Log::info() << "[" << me << "] " << "sending to " << to << " --- " << o << std::endl;

        eckit::MemoryStream s(sendBuffer_);

        o.encode(s);

        eckit::mpi::comm().iSend(static_cast<const char*>(sendBuffer_.data()), sendBuffer_.size(), int(to), 0);
    }

    Obj receive(size_t from) {

        eckit::mpi::Status st = eckit::mpi::comm().probe(int(from), 0);

        size_t size = eckit::mpi::comm().getCount<char>(st);

        eckit::Log::info() << "[" << me << "] " << "receiving from " << from << " --- size " << size << std::endl;

        eckit::Buffer b(size); // must be enough
        b.zero();

        eckit::mpi::comm().receive(static_cast<char*>(b.data()), b.size(), int(from), 0);

        eckit::MemoryStream s(b);

        Obj o(s);

        eckit::Log::info() << "[" << me << "] " << "receiving from " << from << " --- " << o << std::endl;

        return o;
    }

    virtual void run() {

        me = eckit::mpi::comm().rank();

        size_t total = eckit::mpi::comm().size();
        size_t to = circlel(me, total);
        size_t from = circler(me, total);

//        eckit::Log::info() << "[" << me << "] " << from << " -> [" << me << "] -> " << to << std::endl;

        Obj so {"foo", int(me), 374., true};

        send(so, to);

        Obj ro = receive(from);

        ASSERT(ro == Obj("foo", int(from), 374., true));
    }
};

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    ObjSend app(argc, argv);
    int ret = app.start();
    eckit::mpi::finaliseAllComms();
    return ret;
}
