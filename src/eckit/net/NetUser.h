// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_NetUser_h
#define eckit_NetUser_h

#include "eckit/net/TCPSocket.h"
#include "eckit/thread/Thread.h"


namespace eckit {

class Stream;

namespace net {

class NetUser : public Thread {
public:

    NetUser(net::TCPSocket&);

    ~NetUser();

protected:

    TCPSocket protocol_;

private:

    virtual void serve(Stream&, std::istream&, std::ostream&) = 0;

    void run() override;

    friend class NetServiceProcessControler;
};


}  // namespace net
}  // namespace eckit

#endif
