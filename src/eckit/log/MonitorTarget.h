/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file MonitorTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_MonitorTarget_h
#define eckit_log_MonitorTarget_h

#include <iosfwd>
#include <utility>

#include "eckit/log/WrapperTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MonitorTarget : public WrapperTarget {
public:

    MonitorTarget(LogTarget* target = 0);

    ~MonitorTarget() override;

private:

    void write(const char* start, const char* end) override;

    void writePrefix() override {}
    void writeSuffix() override {}

    void print(std::ostream& s) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
