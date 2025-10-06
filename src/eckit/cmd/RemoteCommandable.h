/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

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

class RemoteCommandable : private eckit::NonCopyable {
public:

    // -- Contructors

    RemoteCommandable(int port = 0);

    // -- Destructor

    ~RemoteCommandable();

private:

    // -- Members

    eckit::ThreadControler commander_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
