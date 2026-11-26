// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   Consumer.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_Consumer_H
#define eckit_Consumer_H

#include "eckit/distributed/Actor.h"

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

class Consumer : public Actor {

    virtual void run();

    virtual void getNextMessage(Message& message) const = 0;


public:  // methods

    Consumer(Transport& transport);


    virtual void consume(Message& message) = 0;
    virtual void failure(Message& message);
    virtual void shutdown(Message& message);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed

#endif
