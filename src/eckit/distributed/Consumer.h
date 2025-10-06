/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
