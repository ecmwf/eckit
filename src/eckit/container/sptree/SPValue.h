// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef SPValue_H
#define SPValue_H

#include <iostream>

namespace eckit {

template <class Traits>
class SPValue {
public:

    using Point   = typename Traits::Point;
    using Payload = typename Traits::Payload;

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
