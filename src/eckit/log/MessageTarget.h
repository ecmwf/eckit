// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file MessageTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_MessageTarget_h
#define eckit_log_MessageTarget_h


#include "eckit/log/LineBasedTarget.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MessageTarget : public LineBasedTarget {

public:  // methods

    MessageTarget();

private:

    void line(const char* line) override;
    void print(std::ostream& s) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
