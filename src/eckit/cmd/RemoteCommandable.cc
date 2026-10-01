// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/RemoteCommandable.h"
#include "eckit/cmd/RemoteCommander.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RemoteCommandable::RemoteCommandable(int port) : commander_(new RemoteCommander(port)) {
    commander_.start();
}

//----------------------------------------------------------------------------------------------------------------------

RemoteCommandable::~RemoteCommandable() {
    commander_.stop();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
