// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
