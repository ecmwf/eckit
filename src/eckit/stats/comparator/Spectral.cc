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
#include <memory>
#include <ostream>
#include <sstream>

#include "mir/param/SameParametrisation.h"
#include "mir/stats/statistics/Spectral.h"


namespace mir {
namespace stats {
namespace comparator {


Spectral::Spectral(const param::MIRParametrisation& param1, const param::MIRParametrisation& param2) :
    Comparator(param1, param2),
    meanDiffMax_(std::numeric_limits<double>::quiet_NaN()),
    enormDiffMax_(std::numeric_limits<double>::quiet_NaN()) {
    reset();

    std::unique_ptr<param::MIRParametrisation> param(new param::SameParametrisation(param1, param2, false));
    param->get("spectral-mean-difference-max", meanDiffMax_);
    param->get("spectral-energy-norm-difference-max", enormDiffMax_);
}


void Spectral::reset() {
    meanDiff_  = std::numeric_limits<double>::quiet_NaN();
    enormDiff_ = std::numeric_limits<double>::quiet_NaN();
    stats_.clear();
}


double Spectral::meanDiff() const {
    return meanDiff_;
}


double Spectral::enormDiff() const {
    return enormDiff_;
}


Spectral::~Spectral() = default;


std::string Spectral::execute(const data::MIRField& field1, const data::MIRField& field2) {

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

    std::ostringstream reasons;
    if (meanDiff_ > meanDiffMax_) {
        reasons << "\n" "* difference of spectral mean (" << meanDiff_ << ") greater than " << meanDiffMax_;
    }

    if (enormDiff_ > enormDiffMax_) {
        reasons << "\n" "* difference of spectral energy norm (" << enormDiff_ << ") greater than " << enormDiffMax_;
    }

    return reasons.str();
}


void Spectral::print(std::ostream& out) const {
    out << "Spectral["
            "meanDiff="  << meanDiff()
        << ",enormDiff=" << enormDiff()
        << stats_
        << "]";
}


namespace {
static ComparatorBuilder<Spectral> __stats("spectral");
}


}  // namespace comparator
}  // namespace stats
}  // namespace mir

