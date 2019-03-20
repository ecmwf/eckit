/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/comparator/Spectral.h"

#include <cmath>
#include <limits>
#include <ostream>
#include <sstream>

#include "eckit/memory/ScopedPtr.h"
#include "eckit/parser/JSON.h"

#include "mir/stats/statistics/Spectral.h"


namespace mir {
namespace stats {
namespace comparator {


Spectral::Spectral(const param::MIRParametrisation& parametrisation1, const param::MIRParametrisation& parametrisation2) :
    Comparator(parametrisation1, parametrisation2) {
    reset();
}


void Spectral::reset() {
    meanDiff_  = std::numeric_limits<double>::quiet_NaN();
    enormDiff_ = std::numeric_limits<double>::quiet_NaN();
    stats_.clear();
}


double Spectral::meandiff() const {
    return meanDiff_;
}


double Spectral::enormdiff() const {
    return enormDiff_;
}


Spectral::~Spectral() = default;


void Spectral::execute(const data::MIRField& field1, const data::MIRField& field2) {

    statistics::Spectral stats1(parametrisation1_);
    stats1.execute(field1);

    statistics::Spectral stats2(parametrisation2_);
    stats2.execute(field2);

    meanDiff_ = std::abs(stats2.mean() - stats1.mean());
    enormDiff_ = std::abs(stats2.enorm() - stats1.enorm());

    std::ostringstream str;
    str << ",Field1=" << stats1
        << ",Field2=" << stats2;
    stats_ = str.str();
}


void Spectral::print(std::ostream& out) const {
    out << "Spectral[";
    eckit::JSON j(out);
    j.startObject()
            << "meandiff"  << meandiff()
            << "enormdiff" << enormdiff();
    j.endObject();
    out << stats_ << "]";
}


namespace {
static ComparatorBuilder<Spectral> __stats("spectral");
}


}  // namespace comparator
}  // namespace stats
}  // namespace mir

