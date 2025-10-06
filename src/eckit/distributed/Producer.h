/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
