/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <sstream>

#include "eckit/persist/DumpLoad.h"
#include "eckit/types/Double.h"
#include "eckit/utils/Translator.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Double::Double(double d) :
    value_(d) {}

Double::Double(const std::string& s) :
    value_(Translator<std::string, double>()(s)) {}

void Double::dump(DumpLoad& a) const {
    a.dump(value_);
}

void Double::load(DumpLoad& a) {
    a.load(value_);
}

double Double::round() const {
    double precision = 1000.0;
    // Warning gcc needs two lines
    double x = floor(precision * value_ + 0.5) / precision;
    return x;
}

void Double::print(std::ostream& s) const {
    s << round();
}

Double::operator std::string() const {
    std::ostringstream os;
    os << *this;
    return os.str();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
