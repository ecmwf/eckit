// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/Reverse.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::projection {


namespace {


const Projection& checked(const Projection* projection) {
    ASSERT(projection != nullptr);
    return *projection;
}


}  // namespace


Reverse::Reverse(const Projection* projection) : Projection(checked(projection), true), projection_(projection) {}


Point Reverse::fwd(const Point& p) const {
    return projection_->inv(p);
}


Point Reverse::inv(const Point& q) const {
    return projection_->fwd(q);
}


const std::string& Reverse::type() const {
    return projection_->type();
}


void Reverse::fill_spec(spec::Custom& custom) const {
    projection_->fill_spec(custom);

    if (reverse()) {
        custom.set("reverse", true);
    }
    else {
        custom.erase("reverse");
    }
}


bool Reverse::reverse() const {
    return !projection_->identity() && !projection_->reverse();
}


bool Reverse::identity() const {
    return projection_->identity();
}


std::vector<std::vector<double>> Reverse::fwd_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                     const std::vector<double>& v3) const {
    return projection_->inv(v1, v2, v3);
}


std::vector<std::vector<double>> Reverse::inv_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                     const std::vector<double>& v3) const {
    return projection_->fwd(v1, v2, v3);
}


}  // namespace eckit::geo::projection
