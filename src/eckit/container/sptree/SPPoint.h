/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPPoint_H
#define SPPoint_H

#include <limits>
#include <cmath>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

template<int SIZE = 2>
class SPPoint {

protected:

    double x_[SIZE];

public:

    static const size_t DIMS = SIZE;

    double x(size_t axis) const { return x_[axis]; }

    SPPoint()
    {
        std::fill(x_, x_+dimensions(), 0);
    }

//    SPPoint( double* x )
//    {
//        std::copy(x, x+dimensions(), x_);
//    }

    template<class Container>
    explicit SPPoint(Container c)
    {
        std::copy(c.begin(), c.end(), x_);
    }

    explicit SPPoint(double x, double y)
    {
        ASSERT( SIZE >= 2 );
        x_[0] = x;
        x_[1] = y;
    }

//    explicit SPPoint(double x, double y, double z)
//    {
//        ASSERT( SIZE >= 3 );
//        x_[0] = x;
//        x_[1] = y;
//        x_[2] = z;
//    }

    bool operator<(const SPPoint& other) const
    { return std::lexicographical_compare(x_,x_ + SIZE, other.x_, other.x_ + SIZE); }

    static size_t dimensions()               { return SIZE; }

    friend std::ostream& operator<<(std::ostream& s,const SPPoint& p)
    {
        char z = '{';
        for(size_t i = 0; i < dimensions(); ++i) {
            s << z << p.x_[i]; z = ',';
        }
        s << "}";
        return s;
    }

    static double distance(const SPPoint& p1, const SPPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            double dx =  p1.x_[i]  - p2.x_[i];
            m += dx*dx;
        }
        return std::sqrt(m);
    }

    static bool equal(const SPPoint& p1, const SPPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            if(p1.x_[i]  != p2.x_[i])
                return false;
        }
        return true;
    }

    static double norm(const SPPoint& p1)
    {
        double m = 0.0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            double dx =  p1.x_[i];
            m += dx*dx;
        }
        return std::sqrt(m);
    }

    bool operator==(const SPPoint& other) const {
        return equal(*this, other);
    }

    bool operator!=(const SPPoint& other) const {
        return !equal(*this, other);
    }

    // Distance along one axis
    static double distance(const SPPoint& p1, const SPPoint& p2, int axis)
    {
        return fabs(p1.x_[axis] - p2.x_[axis]);
    }

    // For projecting a point on a line
    static double dot(const SPPoint& p1, const SPPoint& p2)
    {
        double m = 0.0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            m += p1.x_[i] * p2.x_[i];
        }
        return m;
    }

    static SPPoint add(const SPPoint& p1, const SPPoint& p2)
    {
        SPPoint q(p1);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] += p2.x_[i];
        }
        return q;
    }

    static SPPoint middle(const SPPoint& p1, const SPPoint& p2)
    {
        SPPoint q(p1);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] += p2.x_[i];
            q.x_[i] /= 2.0;
        }
        return q;
    }

    static SPPoint sub(const SPPoint& p1, const SPPoint& p2)
    {
        SPPoint q(p1);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] -= p2.x_[i];
        }
        return q;
    }

    static SPPoint mul(const SPPoint& p, double m)
    {
        SPPoint q(p);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] *= m;
        }
        return q;
    }

    static SPPoint div(const SPPoint& p, double m)
    {
        SPPoint q(p);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] /= m;
        }
        return q;
    }

    static SPPoint normalize(const SPPoint& p)
    {
        SPPoint zero;
        return div(p,distance(p, zero));
    }

    template<class Container>
    static typename Container::value_type mean(const Container& points) {

        typename Container::const_iterator j = points.begin();
        typename Container::value_type result(*j);

        ++j;
        size_t count = points.size();

        for(; j != points.end(); ++j) {
            for(size_t i = 0; i < dimensions(); i++) {
                result.point().x_[i] += (*j).point().x_[i];
            }
        }
        for(size_t i = 0; i < dimensions(); i++) {
            result.point().x_[i] /= points.size();
        }
        return result;
    }

    static SPPoint symetrical(const SPPoint& w, const SPPoint& c) {
        SPPoint result(w);
        for(size_t i = 0; i < dimensions(); i++) {
            result.x_[i] -= (c.x_[i] - w.x_[i]);
        }
        return result;
    }

    const double* begin() const { return x_; }
    const double* end() const { return x_ + dimensions(); }


    void normalize(const SPPoint& offset, const SPPoint& scale)
    {
        for(size_t i = 0; i < DIMS; ++i) {
            x_[i] = (x_[i] - offset.x_[i]) / scale.x_[i];
        }
    }

    template<class Container>
    static void normalizeAll(Container& c, SPPoint& offset, SPPoint& scale) {
        std::vector<double> mins(DIMS,  std::numeric_limits<double>::max());
        std::vector<double> maxs(DIMS, -std::numeric_limits<double>::max());

        for(typename Container::const_iterator j = c.begin(); j != c.end(); ++j) {
            const typename Container::value_type& v = (*j);
            for(size_t i = 0; i < DIMS; ++i) {
                mins[i] = std::min(mins[i], v.point().x_[i]);
                maxs[i] = std::max(maxs[i], v.point().x_[i]);
            }
        }

        for(size_t i = 0; i < DIMS; ++i) {
            maxs[i] -= mins[i];
        }


        for(typename Container::iterator j = c.begin(); j != c.end(); ++j) {
            typename Container::value_type& v = (*j);
            for(size_t i = 0; i < DIMS; ++i) {
               v.point().x_[i] = (v.point().x_[i] - mins[i]) / maxs[i];
            }
        }

        offset = SPPoint(mins);
        scale  = SPPoint(maxs);

    }

};

template<int SIZE>
const size_t SPPoint<SIZE>::DIMS;

} // end namespace


#endif
