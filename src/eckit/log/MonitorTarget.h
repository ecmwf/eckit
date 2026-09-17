// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

    MonitorTarget(LogTarget* target = nullptr);

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
