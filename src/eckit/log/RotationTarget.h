// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
