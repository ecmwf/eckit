/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file StatusTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_StatusTarget_h
#define eckit_log_StatusTarget_h


#include "eckit/log/LineBasedTarget.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class StatusTarget : public LineBasedTarget {
public:  // methods

    StatusTarget();

private:

    void line(const char* line) override;
    void print(std::ostream& s) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
