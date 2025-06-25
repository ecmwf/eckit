/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Mar 97

#ifndef eckit_Select_h
#define eckit_Select_h


#include <sys/select.h>

#include "eckit/memory/NonCopyable.h"


namespace eckit {

//-----------------------------------------------------------------------------

namespace net {
class TCPSocket;
};

/// Wraps calls to select
class Select : private NonCopyable {

public:

    // -- Contructors

    Select();
    explicit Select(int);
    explicit Select(net::TCPSocket&);

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
