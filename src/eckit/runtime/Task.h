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
/// @date   May 96

#ifndef eckit_Task_h
#define eckit_Task_h

#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Task : private NonCopyable {
public:

    Task();

    virtual ~Task();

    virtual void start()  = 0;
    virtual void stop()   = 0;
    virtual void wait()   = 0;
    virtual bool active() = 0;
    virtual void kill()   = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
