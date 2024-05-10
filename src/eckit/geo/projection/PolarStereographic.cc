/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/projection/PolarStereographic.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"


namespace eckit::geo::projection {


PolarStereographic::PolarStereographic(const Spec&) {}


Point PolarStereographic::fwd(const Point&) const {
    NOTIMP;
}


Point PolarStereographic::inv(const Point&) const {
    NOTIMP;
}


Spec* PolarStereographic::spec() const {
    NOTIMP;
}


#if 0
static constexpr double EPSILON = 1e-10;


/* Data struct for Forward and Inverse Projections */
typedef struct proj_data_t {
    double centre_lon;     /* central longitude */
    double centre_lat;     /* central latitude */
    double sign;           /* sign variable */
    double ind;            /* flag variable */
    double mcs;            /* small m */
    double tcs;            /* small t */
    double false_northing; /* y offset in meters */
    double false_easting;  /* x offset in meters */
} proj_data_t;


static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args) {
    int ret = 0;
    double *lats, *lons; /* arrays for latitudes and longitudes */
    double lonFirstInDegrees, latFirstInDegrees, radius;
    double x, y, Dx, Dy;
    long nx, ny;
    double centralLongitudeInDegrees, centralLatitudeInDegrees;
    long alternativeRowScanning, iScansNegatively, i, j;
    long jScansPositively, jPointsAreConsecutive, southPoleOnPlane;
    double centralLongitude, centralLatitude; /* in radians */
    double con1;                              /* temporary angle */
    double ts;                                /* value of small t */
    double height;                            /* height above ellipsoid */
    double x0, y0, lonFirst, latFirst;
    proj_data_t fwd_proj_data = {
        0,
    };
    proj_data_t inv_proj_data = {
        0,
    };

    grib_iterator_polar_stereographic* self = (grib_iterator_polar_stereographic*)iter;

    const char* s_radius                 = grib_arguments_get_name(h, args, self->carg++);
    const char* s_nx                     = grib_arguments_get_name(h, args, self->carg++);
    const char* s_ny                     = grib_arguments_get_name(h, args, self->carg++);
    const char* s_latFirstInDegrees      = grib_arguments_get_name(h, args, self->carg++);
    const char* s_lonFirstInDegrees      = grib_arguments_get_name(h, args, self->carg++);
    const char* s_southPoleOnPlane       = grib_arguments_get_name(h, args, self->carg++);
    const char* s_centralLongitude       = grib_arguments_get_name(h, args, self->carg++);
    const char* s_centralLatitude        = grib_arguments_get_name(h, args, self->carg++);
    const char* s_Dx                     = grib_arguments_get_name(h, args, self->carg++);
    const char* s_Dy                     = grib_arguments_get_name(h, args, self->carg++);
    const char* s_iScansNegatively       = grib_arguments_get_name(h, args, self->carg++);
    const char* s_jScansPositively       = grib_arguments_get_name(h, args, self->carg++);
    const char* s_jPointsAreConsecutive  = grib_arguments_get_name(h, args, self->carg++);
    const char* s_alternativeRowScanning = grib_arguments_get_name(h, args, self->carg++);

    if (grib_is_earth_oblate(h)) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Only supported for spherical earth.", ITER);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    if ((ret = grib_get_double_internal(h, s_radius, &radius)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, s_nx, &nx)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, s_ny, &ny)) != GRIB_SUCCESS)
        return ret;

    if (iter->nv != nx * ny) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Wrong number of points (%zu!=%ldx%ld)", ITER, iter->nv, nx,
                         ny);
        return GRIB_WRONG_GRID;
    }
    if ((ret = grib_get_double_internal(h, s_latFirstInDegrees, &latFirstInDegrees)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, s_lonFirstInDegrees, &lonFirstInDegrees)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, s_southPoleOnPlane, &southPoleOnPlane)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, s_centralLongitude, &centralLongitudeInDegrees)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, s_centralLatitude, &centralLatitudeInDegrees)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, s_Dx, &Dx)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, s_Dy, &Dy)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, s_jPointsAreConsecutive, &jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, s_jScansPositively, &jScansPositively)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, s_iScansNegatively, &iScansNegatively)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, s_alternativeRowScanning, &alternativeRowScanning)) != GRIB_SUCCESS)
        return ret;

    centralLongitude = centralLongitudeInDegrees * util::DEGREE_TO_RADIAN;
    centralLatitude  = centralLatitudeInDegrees * util::DEGREE_TO_RADIAN;
    lonFirst         = lonFirstInDegrees * util::DEGREE_TO_RADIAN;
    latFirst         = latFirstInDegrees * util::DEGREE_TO_RADIAN;

    /* Forward projection initialisation */
    fwd_proj_data.false_northing = 0;
    fwd_proj_data.false_easting  = 0;
    fwd_proj_data.centre_lon     = centralLongitude;
    fwd_proj_data.centre_lat     = centralLatitude;
    if (centralLatitude < 0)
        fwd_proj_data.sign = -1.0;
    else
        fwd_proj_data.sign = +1.0;
    fwd_proj_data.ind = 0;
    if (fabs(fabs(centralLatitude) - M_PI_2) > EPSILON) {
        /* central latitude different from 90 i.e. not north/south polar */
        fwd_proj_data.ind = 1;
        con1              = fwd_proj_data.sign * centralLatitude;
        fwd_proj_data.mcs = cos(con1);
        fwd_proj_data.tcs = tan(0.5 * (M_PI_2 - con1));
    }

    /* Forward projection from initial lat,lon to initial x,y */
    con1 = fwd_proj_data.sign * (lonFirst - fwd_proj_data.centre_lon);
    ts   = tan(0.5 * (M_PI_2 - fwd_proj_data.sign * latFirst));
    if (fwd_proj_data.ind)
        height = radius * fwd_proj_data.mcs * ts / fwd_proj_data.tcs;
    else
        height = 2.0 * radius * ts;
    x0 = fwd_proj_data.sign * height * sin(con1) + fwd_proj_data.false_easting;
    y0 = -fwd_proj_data.sign * height * cos(con1) + fwd_proj_data.false_northing;

    x0 = -x0;
    y0 = -y0;

    /* Inverse projection initialisation */
    inv_proj_data.false_easting  = x0;
    inv_proj_data.false_northing = y0;
    inv_proj_data.centre_lon     = centralLongitude;
    inv_proj_data.centre_lat     = centralLatitude;
    if (centralLatitude < 0)
        inv_proj_data.sign = -1.0;
    else
        inv_proj_data.sign = +1.0;
    inv_proj_data.ind = 0;
    if (fabs(fabs(centralLatitude) - M_PI_2) > EPSILON) {
        inv_proj_data.ind = 1;
        con1              = inv_proj_data.sign * inv_proj_data.centre_lat;
        inv_proj_data.mcs = cos(con1);
        inv_proj_data.tcs = tan(0.5 * (M_PI_2 - con1));
    }
    self->lats = (double*)grib_context_malloc(h->context, iter->nv * sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, iter->nv * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    self->lons = (double*)grib_context_malloc(h->context, iter->nv * sizeof(double));
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, iter->nv * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    lats = self->lats;
    lons = self->lons;
    /* These will be processed later in transform_iterator_data() */
    /* Dx = iScansNegatively == 0 ? Dx : -Dx; */
    /* Dy = jScansPositively == 1 ? Dy : -Dy; */

    y = 0;
    for (j = 0; j < ny; j++) {
        x = 0;
        for (i = 0; i < nx; i++) {
            /* Inverse projection from x,y to lat,lon */
            /* int index =i+j*nx; */
            double _x = (x - inv_proj_data.false_easting) * inv_proj_data.sign;
            double _y = (y - inv_proj_data.false_northing) * inv_proj_data.sign;
            double rh = sqrt(_x * _x + _y * _y);
            if (inv_proj_data.ind)
                ts = rh * inv_proj_data.tcs / (radius * inv_proj_data.mcs);
            else
                ts = rh / (radius * 2.0);
            *lats = inv_proj_data.sign * (M_PI_2 - 2 * atan(ts));
            if (rh == 0) {
                *lons = inv_proj_data.sign * inv_proj_data.centre_lon;
            }
            else {
                double temp = atan2(_x, -_y);
                *lons       = inv_proj_data.sign * temp + inv_proj_data.centre_lon;
            }
            *lats = *lats * util::RADIAN_TO_DEGREE;
            *lons = *lons * util::RADIAN_TO_DEGREE;
            while (*lons < 0)
                *lons += 360;
            while (*lons > 360)
                *lons -= 360;
            lons++;
            lats++;

            x += Dx;
        }
        y += Dy;
    }

    iter->e = -1;

    /* Apply the scanning mode flags which may require data array to be transformed */
    ret = transform_iterator_data(h->context, iter->data, iScansNegatively, jScansPositively, jPointsAreConsecutive,
                                  alternativeRowScanning, iter->nv, nx, ny);

    return ret;
}
#endif


}  // namespace eckit::geo::projection
