/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    typedef std::ostream& (*colourproc)(std::ostream&);

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
