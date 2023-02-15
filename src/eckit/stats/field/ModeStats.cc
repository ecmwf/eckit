/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/field/ModeStats.h"

#include <ostream>

#include "mir/stats/detail/ModeT.h"


namespace mir::stats::field {


struct ModeReal final : ModeStatsT<detail::ModeReal> {
    using ModeStatsT::ModeStatsT;
    double value() const override { return mode(); }
    void print(std::ostream& out) const override { out << "ModeReal[" << value() << "]"; }
};


struct ModeIntegral final : ModeStatsT<detail::ModeIntegral> {
    using ModeStatsT::ModeStatsT;
    double value() const override { return mode(); }
    void print(std::ostream& out) const override { out << "ModeIntegral[" << value() << "]"; }
};


struct ModeBoolean final : ModeStatsT<detail::ModeBoolean> {
    using ModeStatsT::ModeStatsT;
    double value() const override { return mode(); }
    void print(std::ostream& out) const override { out << "ModeBoolean[" << value() << "]"; }
};


struct MedianIntegral final : ModeStatsT<detail::MedianIntegral> {
    using ModeStatsT::ModeStatsT;
    double value() const override { return median(); }
    void print(std::ostream& out) const override { out << "MedianIntegral[" << value() << "]"; }
};


static const FieldBuilder<ModeReal> __builder_1("mode-real");
static const FieldBuilder<ModeIntegral> __builder_2("mode-integral");
static const FieldBuilder<ModeBoolean> __builder_3("mode-boolean");

static const FieldBuilder<MedianIntegral> __builder_4("median-integral");
static const FieldBuilder<ModeBoolean> __builder_5("median-boolean");  // (alias)


}  // namespace mir::stats::field
