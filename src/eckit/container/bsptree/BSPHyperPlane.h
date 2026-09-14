// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef BSPHyperPlane_H
#define BSPHyperPlane_H


namespace eckit {


template <class Point>
class BSPHyperPlane {

    Point normal_;
    double d_;

public:

    BSPHyperPlane() : normal_(), d_() {}

    BSPHyperPlane(const Point& normal, const Point& point) :
        normal_(Point::normalize(normal)), d_(-Point::dot(normal_, point)) {}

    double position(const Point& p) const { return Point::dot(p, normal_) + d_; }

    const Point& normal() const { return normal_; }
    double d() const { return d_; }
};


}  // namespace eckit


#endif
