// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   RemoteCommand.h
/// @author Manuel Fuentes
/// @date   Jul 1996

#ifndef eckit_cmd_RemoteCommandable_H
#define eckit_cmd_RemoteCommandable_H

#include "eckit/thread/ThreadControler.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

/// A RemoteCommand-able object

class RemoteCommandable {
public:

    // -- Contructors

    RemoteCommandable(int port = 0);

    RemoteCommandable(const RemoteCommandable&)            = delete;
    RemoteCommandable& operator=(const RemoteCommandable&) = delete;
    RemoteCommandable(RemoteCommandable&&)                 = delete;
    RemoteCommandable& operator=(RemoteCommandable&&)      = delete;

    // -- Destructor

    ~RemoteCommandable();

private:

    // -- Members

    eckit::ThreadControler commander_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
