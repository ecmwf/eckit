/*
 * (C) Copyright 1996- ECMWF.
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

#include <string>

#include "eckit/log/LogTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RotationTarget : public LogTarget {

public:  // methods

    RotationTarget(const std::string& name = std::string());

    ~RotationTarget() override;

    void write(const char* start, const char* end) override;
    void flush() override;

protected:

    void print(std::ostream& s) const override;

private:

    std::string name_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
