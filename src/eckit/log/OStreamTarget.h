// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file OStreamTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_OStreamTarget_h
#define eckit_log_OStreamTarget_h


#include "eckit/log/LogTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class OStreamTarget : public LogTarget {

public:  // methods

    OStreamTarget(std::ostream& out);

    ~OStreamTarget() override;

    void write(const char* start, const char* end) override;
    void flush() override;
    void print(std::ostream& s) const override;

private:

    std::ostream& out_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
