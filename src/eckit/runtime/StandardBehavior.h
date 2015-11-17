/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_StandardBehavior_h
#define eckit_StandardBehavior_h

#include "eckit/runtime/ContextBehavior.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StandardBehavior : public ContextBehavior {
public:

    /// Contructors

    StandardBehavior();

    /// Destructor

    ~StandardBehavior();

public: // methods

    virtual Channel& infoChannel();
    virtual Channel& warnChannel();
    virtual Channel& errorChannel();
    virtual Channel& debugChannel();
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_StandardBehavior_h
