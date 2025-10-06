/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPValue_H
#define SPValue_H

#include <iostream>

namespace eckit {

template <class Traits>
class SPValue {
public:

    typedef typename Traits::Point Point;
    typedef typename Traits::Payload Payload;

    Point point_;
    Payload payload_;

public:

    SPValue(const Point& point, const Payload& payload) : point_(point), payload_(payload) {}

    template <class V>
    SPValue(const V& v) : point_(v.point()), payload_(v.payload()) {}

    const Point& point() const { return point_; }
    const Payload& payload() const { return payload_; }


    Point& point() { return point_; }  // FIXME: remove this one
    Payload& payload() { return payload_; }


    void point(const Point& p) const { point_ = p; }
    void payload(const Payload& p) const { payload_ = p; }

    void print(std::ostream& o) const { o << "(point=" << point_ << ",payload=" << payload_ << ")"; }

    friend std::ostream& operator<<(std::ostream& o, const SPValue& t) {
        t.print(o);
        return o;
    }

    bool operator<(const SPValue& other) const { return point() < other.point(); }
};

}  // namespace eckit


#endif
