// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date Mar 97

#ifndef eckit_Select_h
#define eckit_Select_h


#include <sys/select.h>


namespace eckit {

//-----------------------------------------------------------------------------

namespace net {
class TCPSocket;
};

/// Wraps calls to select
class Select {

public:

    // -- Contructors

    Select();
    explicit Select(int);
    explicit Select(net::TCPSocket&);

    Select(const Select&)            = delete;
    Select& operator=(const Select&) = delete;
    Select(Select&&)                 = delete;
    Select& operator=(Select&&)      = delete;

    // -- Destructor

    ~Select();

    // -- Methods

    bool ready(long sec = 20);

    void add(net::TCPSocket&);
    void add(int);

    void remove(net::TCPSocket&);
    void remove(int);

    bool set(net::TCPSocket&);
    bool set(int);

private:

    // -- Members

    fd_set files_;
    fd_set set_;
    int last_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
