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

#include "mir/param/MIRParametrisation.h"
#include "mir/stats/detail/ModeT.h"


namespace mir {
namespace stats {
namespace field {


struct ModeReal final : ModeStatsT<detail::ModeReal> {
    ModeReal(const param::MIRParametrisation& param) : ModeStatsT(param) {
        // TODO
    }

    double value() const override { return mode(); }
    void print(std::ostream& out) const override { out << "ModeReal[]"; }
};


struct ModeIntegral final : ModeStatsT<detail::ModeIntegral> {
    using ModeStatsT::ModeStatsT;
    double value() const override { return mode(); }
    void print(std::ostream& out) const override { out << "ModeIntegral[]"; }
};


struct ModeBoolean final : ModeStatsT<detail::ModeBoolean> {
    ModeBoolean(const param::MIRParametrisation& param) : ModeStatsT(param) {
        // TODO
    }

    double value() const override { return mode(); }
    void print(std::ostream& out) const override { out << "ModeBoolean[]"; }
};


static FieldBuilder<ModeIntegral> __builder_1("mode-integral");
static FieldBuilder<ModeReal> __builder_2("mode-real");
static FieldBuilder<ModeBoolean> __builder_3("mode-boolean");


}  // namespace field
}  // namespace stats
}  // namespace mir
