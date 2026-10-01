// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   Producer.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#ifndef eckit_Producer_H
#define eckit_Producer_H

#include "eckit/distributed/Actor.h"

namespace eckit::distributed {

//----------------------------------------------------------------------------------------------------------------------

class Producer : public Actor {

public:  // methods

    Producer(Transport& transport);

    virtual void run();

    virtual bool produce(Message& message) = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::distributed

#endif
