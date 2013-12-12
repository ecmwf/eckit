/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KDPoint_H
#define KDPoint_H

#include "eckit/eckit.h"

#include <limits>
#include <cmath>


namespace eckit {


template<class T, int SIZE = 2>
class KDPoint {
protected:
    double x_[SIZE];

public:

    typedef T Payload;

    Payload payload_;

    double x(size_t axis) const { return x_[axis]; }

    KDPoint(): payload_()
    {
        std::fill(x_, x_+dimensions(), 0);
    }

    KDPoint(const Payload& payload): payload_(payload)
    {
        std::fill(x_, x_+dimensions(), 0);
    }

    KDPoint(double x, double y, const Payload& payload): payload_(payload)
    {
        x_[0] = x;
        x_[1] = y;
    }
    
    bool operator<(const KDPoint& other) const
    { return std::lexicographical_compare(x_,x_ + SIZE, other.x_, other.x_ + SIZE); }

    //static size_t dimensions(const KDPoint&) { return SIZE; }
    static size_t dimensions()               { return SIZE; }

    friend std::ostream& operator<<(std::ostream& s,const KDPoint& p)
    {
        s << '(' << p.x_[0] << "," << p.x_[1] << ' ' << p.payload_ << ')';
        //s << '(' << p.x_[0] << "," << p.x_[1] << ')';
        return s;
    }

    static  double distance(const KDPoint& p1, const KDPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            double dx =  p1.x_[i]  - p2.x_[i];
            m += dx*dx;
        }
        return std::sqrt(m);
    }

    static  bool equal(const KDPoint& p1, const KDPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < dimensions(); i++)
        {
           if(p1.x_[i]  != p2.x_[i])
               return false;
        }
        return true;
    }

    bool operator==(const KDPoint& other) const {
        return equal(*this, other);
    }

    bool operator!=(const KDPoint& other) const {
        return !equal(*this, other);
    }

    // Distance along one axis
    static  double distance(const KDPoint& p1, const KDPoint& p2, int axis)
    {
        return fabs(p1.x_[axis] - p2.x_[axis]);
    }

    // For projecting a point on a line
    static double dot(const KDPoint& p1, const KDPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < dimensions(); i++)
        {
            m += p1.x_[i] * p2.x_[i];
        }
        return m;
    }

    static KDPoint add(const KDPoint& p1, const KDPoint& p2)
    {
        KDPoint q(p1);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] += p2.x_[i];
        }
        return q;
    }

    static KDPoint middle(const KDPoint& p1, const KDPoint& p2)
    {
        KDPoint q(p1);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] += p2.x_[i];
            q.x_[i] /= 2.0;
        }
        return q;
    }

    static KDPoint sub(const KDPoint& p1, const KDPoint& p2)
    {
        KDPoint q(p1);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] -= p2.x_[i];
        }
        return q;
    }

    static KDPoint mul(const KDPoint& p, double m)
    {
        KDPoint q(p);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] *= m;
        }
        return q;
    }

    static KDPoint div(const KDPoint& p, double m)
    {
        KDPoint q(p);
        for(size_t i = 0; i < dimensions(); i++)
        {
            q.x_[i] /= m;
        }
        return q;
    }

    static KDPoint normalize(const KDPoint& p)
    {
        KDPoint zero;
        return mul(p,distance(p, zero));
    }

    template<class Container>
    static typename Container::value_type mean(const Container& points) {

        typename Container::const_iterator j = points.begin();
        typename Container::value_type result(*j);

        ++j;
        size_t count = points.size();

        for(; j != points.end(); ++j) {
            for(size_t i = 0; i < dimensions(); i++) {
                result.x_[i] += (*j).x_[i];
            }
        }
        for(size_t i = 0; i < dimensions(); i++) {
            result.x_[i] /= points.size();
        }
        return result;
    }

    static KDPoint symetrical(const KDPoint& w, const KDPoint& c) {
        KDPoint result(w);
        for(size_t i = 0; i < dimensions(); i++) {
            result.x_[i] -= (c.x_[i] - w.x_[i]);
        }
        return result;
    }

    const double* begin() const { return x_; }
    const double* end() const { return x_ + dimensions(); }

};



} // Name space


#endif
