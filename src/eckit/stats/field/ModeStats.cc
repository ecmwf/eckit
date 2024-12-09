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


#include "eckit/stats/field/ModeStats.h"

#include <ostream>

#include "eckit/log/JSON.h"

#include "eckit/stats/detail/ModeT.h"


namespace eckit::stats::field {


struct ModeReal final : ModeStatsT<detail::ModeReal> {
    using ModeStatsT::ModeStatsT;
    double value() const override { return mode(); }
    void json(eckit::JSON& j) const override { json_tv(j, "mode-real", value()); }
    void print(std::ostream& out) const override { out << "ModeReal[" << value() << "]"; }
};


struct ModeIntegral final : ModeStatsT<detail::ModeIntegral> {
    using ModeStatsT::ModeStatsT;
    double value() const override { return mode(); }
    void json(eckit::JSON& j) const override { json_tv(j, "mode-integral", value()); }
    void print(std::ostream& out) const override { out << "ModeIntegral[" << value() << "]"; }
};


struct ModeBoolean final : ModeStatsT<detail::ModeBoolean> {
    using ModeStatsT::ModeStatsT;
    double value() const override { return mode(); }
    void json(eckit::JSON& j) const override { json_tv(j, "mode-boolean", value()); }
    void print(std::ostream& out) const override { out << "ModeBoolean[" << value() << "]"; }
};


struct MedianIntegral final : ModeStatsT<detail::MedianIntegral> {
    using ModeStatsT::ModeStatsT;
    double value() const override { return median(); }
    void json(eckit::JSON& j) const override { json_tv(j, "median-integral", value()); }
    void print(std::ostream& out) const override { out << "MedianIntegral[" << value() << "]"; }
};


namespace {
const FieldStatisticsFactory* FIELDSTATISTICS[]{
    new FieldStatisticsBuilder<ModeReal>("mode-real"),
    new FieldStatisticsBuilder<ModeIntegral>("mode-integral"),
    new FieldStatisticsBuilder<ModeBoolean>("mode-boolean"),
    new FieldStatisticsBuilder<MedianIntegral>("median-integral"),
    new FieldStatisticsBuilder<ModeBoolean>("median-boolean"),  // (alias)
};
}


}  // namespace eckit::stats::field
