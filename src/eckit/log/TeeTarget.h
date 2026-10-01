// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file TeeTarget.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#ifndef eckit_log_TeeTarget_h
#define eckit_log_TeeTarget_h


#include "eckit/log/LogTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TeeTarget : public LogTarget {

public:  // methods

    TeeTarget(LogTarget* left, LogTarget* right);

    ~TeeTarget() override;

protected:

    void print(std::ostream& s) const override;

private:

    LogTarget* left_;
    LogTarget* right_;

private:

    void write(const char* start, const char* end) override;
    void flush() override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
