// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file ColouringTarget.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#ifndef eckit_log_ColouringTarget_h
#define eckit_log_ColouringTarget_h

#include <iosfwd>
#include <string>

#include "eckit/log/Colour.h"
#include "eckit/log/WrapperTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ColouringTarget : public WrapperTarget {
public:

    using colourproc = std::ostream& (*)(std::ostream&);

    ColouringTarget(LogTarget* target, colourproc begin, colourproc end = &Colour::reset);

    ~ColouringTarget() override;

protected:

    void print(std::ostream& s) const override;

private:

    void writePrefix() override;
    void writeSuffix() override;

    std::string begin_;
    std::string end_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
