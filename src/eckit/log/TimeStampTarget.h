// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file TimeStampTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_TimeStampTarget_h
#define eckit_log_TimeStampTarget_h

#include <utility>

#include "eckit/log/WrapperTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class TimeStampTarget : public WrapperTarget {
public:

    TimeStampTarget(const char* tag = "", LogTarget* target = nullptr);

    ~TimeStampTarget() override;

protected:

    void print(std::ostream& s) const override;

private:

    void writePrefix() override;
    void writeSuffix() override;

private:

    const char* tag_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
