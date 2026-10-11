// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/Reverse.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::projection {


namespace {


Figure* figure_copy(const Projection* projection) {
    ASSERT(projection != nullptr);
    return FigureFactory::build(*std::unique_ptr<spec::Custom>(projection->figure().spec()));
}


}  // namespace


Reverse::Reverse(const Projection* projection) : Projection(figure_copy(projection)), projection_(projection) {
    point_types_from(*projection_, *projection_);
    reverse_point_types();
}


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
    if (const auto* reversed = dynamic_cast<const Reverse*>(projection_.get()); reversed != nullptr) {
        reversed->projection_->fill_spec(custom);
        return;
    }

    projection_->fill_spec(custom);
    custom.set("reverse", true);
}


bool Reverse::reverse() const {
    return dynamic_cast<const Reverse*>(projection_.get()) == nullptr;
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
