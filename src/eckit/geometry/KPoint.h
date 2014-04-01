/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KPoint_H
#define KPoint_H

#include <cassert>
#include <limits>
#include <cmath>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace geometry {

//------------------------------------------------------------------------------------------------------

enum COORDS { XX = 0, YY = 1, ZZ = 2 };

/// A generic point in K dimension cartesian space

template<int SIZE = 2>
class KPoint {

protected:

    double x_[SIZE];

public:

    static const size_t DIMS = SIZE;

    double x(size_t axis) const { return x_[axis]; }

    KPoint()
    {
        std::fill(x_, x_+dimensions(), 0);
    }

    KPoint( double* x )
    {
        std::copy(x, x+dimensions(), x_);
    }

    template<class Container>
    explicit KPoint(Container c)
    {
        std::copy(c.begin(), c.end(), x_);
    }

    explicit KPoint(double x, double y)
    {
        ASSERT( SIZE >= 2 );
        x_[0] = x;
        x_[1] = y;
    }

//    explicit KPoint(double x, double y, double z)
//    {
//        ASSERT( SIZE >= 3 );
//        x_[0] = x;
//        x_[1] = y;
//        x_[2] = z;
//    }

    double* data() { return x_; }

    double operator()( const size_t& i ) const { assert( i < SIZE ); return x_[i]; }

    bool operator<(const KPoint& other) const
    { return std::lexicographical_compare(x_,x_ + SIZE, other.x_, other.x_ + SIZE); }

    static size_t dimensions() { return SIZE; }

    friend std::ostream& operator<<(std::ostream& s,const KPoint& p)
    {
        char z = '{';
        for(size_t i = 0; i < dimensions(); ++i) {
            s << z << p.x_[i]; z = ',';
        }
        s << "}";
        return s;
    }

    static double distance(const KPoint& p1, const KPoint& p2)
    {
        double d = 0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            double dx =  p1.x_[i]  - p2.x_[i];
            d += dx*dx;
        }
        return std::sqrt(d);
    }

    static double distance2(const KPoint& p1, const KPoint& p2)
    {
        double d = 0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            double dx =  p1.x_[i]  - p2.x_[i];
            d += dx*dx;
        }
        return d;
    }

    static bool equal(const KPoint& p1, const KPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            if(p1.x_[i]  != p2.x_[i])
                return false;
        }
        return true;
    }

    static double norm(const KPoint& p1)
    {
        double n = 0.0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            double dx =  p1.x_[i];
            n += dx*dx;
        }
        return std::sqrt(n);
    }

    bool operator==(const KPoint& other) const {
        return equal(*this, other);
    }

    bool operator!=(const KPoint& other) const {
        return !equal(*this, other);
    }

    // Distance along one axis
    static double distance(const KPoint& p1, const KPoint& p2, int axis)
    {
        return std::abs(p1.x_[axis] - p2.x_[axis]);
    }

    // For projecting a point on a line
    static double dot(const KPoint& p1, const KPoint& p2)
    {
        double m = 0.0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            m += p1.x_[i] * p2.x_[i];
        }
        return m;
    }

    static KPoint add(const KPoint& p1, const KPoint& p2)
    {
        KPoint q(p1);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] += p2.x_[i];
        }
        return q;
    }

    static KPoint middle(const KPoint& p1, const KPoint& p2)
    {
        KPoint q(p1);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] += p2.x_[i];
            q.x_[i] /= 2.0;
        }
        return q;
    }

    static KPoint sub(const KPoint& p1, const KPoint& p2)
    {
        KPoint q(p1);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] -= p2.x_[i];
        }
        return q;
    }

    static KPoint mul(const KPoint& p, double m)
    {
        KPoint q(p);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] *= m;
        }
        return q;
    }

    static KPoint div(const KPoint& p, double m)
    {
        KPoint q(p);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] /= m;
        }
        return q;
    }

    static KPoint normalize(const KPoint& p)
    {
        KPoint zero;
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

    static KPoint symetrical(const KPoint& w, const KPoint& c) {
        KPoint result(w);
        for(size_t i = 0; i < dimensions(); i++) {
            result.x_[i] -= (c.x_[i] - w.x_[i]);
        }
        return result;
    }

    const double* begin() const { return x_; }
    const double* end() const { return x_ + dimensions(); }


    void normalize(const KPoint& offset, const KPoint& scale)
    {
        for(size_t i = 0; i < DIMS; ++i) {
            x_[i] = (x_[i] - offset.x_[i]) / scale.x_[i];
        }
    }

    template<class Container>
    static void normalizeAll(Container& c, KPoint& offset, KPoint& scale) {
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

        offset = KPoint(mins);
        scale  = KPoint(maxs);

    }

};

//------------------------------------------------------------------------------------------------------

template< int SIZE > const size_t KPoint<SIZE>::DIMS;

//------------------------------------------------------------------------------------------------------

} // namespace geometry
} // namespace eckit

#endif
