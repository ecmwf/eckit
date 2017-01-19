/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file RotationTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_RotationTarget_h
#define eckit_log_RotationTarget_h


#include "eckit/log/LogTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RotationTarget : public LogTarget {

public: // methods

    RotationTarget();

    virtual ~RotationTarget();

    virtual void write(const char* start, const char* end);
    virtual void flush();

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
