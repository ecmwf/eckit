/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef BSPPoint_H
#define BSPPoint_H

#include "eckit/eckit.h"

#include <limits>
#include <cmath>


namespace eckit {


template<class T, int SIZE = 2>
class BSPPoint {
protected:
    double x_[SIZE];

public:

    typedef T Payload;

    Payload payload_;

    double x(int axis) const { return x_[axis]; }

    BSPPoint(): payload_()
    {
        std::fill(x_, x_+size(*this), 0);
    }

    BSPPoint(double x, double y, const Payload& payload): payload_(payload)
    {
        x_[0] = x;
        x_[1] = y;
    }
    
    bool operator<(const BSPPoint& other) const
    { return lexicographical_compare(x_,x_ + SIZE, other.x_, other.x_ + SIZE); }

    static size_t size(const BSPPoint&) { return SIZE; }

    friend std::ostream& operator<<(std::ostream& s,const BSPPoint& p)
    {
        s << '(' << p.x_[0] << "," << p.x_[1] << ' ' << p.payload_ << ')';
        //s << '(' << p.x_[0] << "," << p.x_[1] << ')';
        return s;
    }

    static  double distance(const BSPPoint& p1, const BSPPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < size(p1); i++)
        {
            double dx =  p1.x_[i]  - p2.x_[i];
            m += dx*dx;
        }
        return std::sqrt(m);
    }

    // Distance along one axis
    static  double distance(const BSPPoint& p1, const BSPPoint& p2, int axis)
    {
        return fabs(p1.x_[axis] - p2.x_[axis]);
    }

    // For projecting a point on a line
    static double dot(const BSPPoint& p1, const BSPPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < size(); i++)
        {
            m += p1.x_[i] * p2.x_[i];
        }
        return m;
    }

    static BSPPoint add(const BSPPoint& p1, const BSPPoint& p2)
    {
        BSPPoint q(p1);
        for(size_t i = 0; i < size(); i++)
        {
            q.x_[i] += p2.x_[i];
        }
        return q;
    }

    static BSPPoint sub(const BSPPoint& p1, const BSPPoint& p2)
    {
        BSPPoint q(p1);
        for(size_t i = 0; i < size(); i++)
        {
            q.x_[i] -= p2.x_[i];
        }
        return q;
    }

    static BSPPoint mul(const BSPPoint& p, double m)
    {
        BSPPoint q(p);
        for(size_t i = 0; i < size(); i++)
        {
            q.x_[i] *= m;
        }
        return q;
    }

    static BSPPoint div(const BSPPoint& p, double m)
    {
        BSPPoint q(p);
        for(size_t i = 0; i < size(); i++)
        {
            q.x_[i] /= m;
        }
        return q;
    }

    static BSPPoint normalize(const BSPPoint& p)
    {
        BSPPoint zero;
        return mul(p,distance(p, zero));
    }

    /*

    u = diff(a,b);
    v = diff(a,p);

    U = normalize(u);
    x = mul(U,dot(U,v));

    // P = project(p) on line(a,b)
    P = add(a,x)

    // Distance to line
    ditance(p,P)



    */

};



} // Name space


#endif
