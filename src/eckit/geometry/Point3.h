#ifndef eckit_geometry_Point3_h
#define eckit_geometry_Point3_h

#include "eckit/geometry/KPoint.h"

//------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//------------------------------------------------------------------------------------------------------

class Point3 : public eckit::geometry::KPoint<3> {

    typedef KPoint<3> BasePoint;

public:

    Point3() : BasePoint() {}

    Point3(const BasePoint& p) : BasePoint(p) {}

    Point3(const double* p) : BasePoint(p) {}

    Point3(double x, double y, double z) : BasePoint() {
        x_[XX] = x;
        x_[YY] = y;
        x_[ZZ] = z;
    }

    double operator[](const size_t& i) const {
        assert(i < 3);
        return x_[i];
    }
    double& operator[](const size_t& i) {
        assert(i < 3);
        return x_[i];
    }

    template <typename T>
    void assign(const T& p) {
        x_[XX] = p[XX];
        x_[YY] = p[YY];
        x_[ZZ] = p[ZZ];
    }

    static Point3 cross(const Point3& p1, const Point3& p2) {
        return Point3(p1[YY] * p2[ZZ] - p1[ZZ] * p2[YY], p1[ZZ] * p2[XX] - p1[XX] * p2[ZZ],
                      p1[XX] * p2[YY] - p1[YY] * p2[XX]);
    }
};

//------------------------------------------------------------------------------------------------------

bool points_equal(const Point3&, const Point3&);

//------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry

#endif
