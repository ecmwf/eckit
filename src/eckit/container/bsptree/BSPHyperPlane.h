/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
