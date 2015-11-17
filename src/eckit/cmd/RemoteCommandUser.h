/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   RemoteCommandUser.h
/// @author Manuel Fuentes
/// @date   Jul 1996

#ifndef eckit_cmd_RemoteCommandUser_H
#define eckit_cmd_RemoteCommandUser_H

#include "eckit/net/NetUser.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class RemoteCommandUser : public eckit::NetUser {
public:
    RemoteCommandUser(eckit::TCPSocket&);
    virtual ~RemoteCommandUser();
    static void terminate(RemoteCommandUser& other) { other.stop(); }

private:
    virtual void serve(eckit::Stream&, std::istream&, std::ostream&);
    std::string from_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
