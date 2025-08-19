/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Sep 2012

#ifndef eckit_Library_h
#define eckit_Library_h

#include "eckit/runtime/Main.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Library : public Main {

public:  // methods

    /// Contructors

    Library(int argc, char** argv, const char* homeenv = nullptr);

    /// Destructor

    ~Library() override;

protected:

    virtual Channel* createChannel() const;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
