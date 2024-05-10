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


#include "eckit/geo/projection/SpaceView.h"

#include <cmath>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"


namespace eckit::geo::projection {


SpaceView::SpaceView(const Spec&) {}


Point SpaceView::fwd(const Point&) const {
    NOTIMP;
}


Point SpaceView::inv(const Point&) const {
    NOTIMP;
}


Spec* SpaceView::spec() const {
    NOTIMP;
}


#if 0
static int init(grib_iterator* iter, grib_handle* h, grib_arguments* args) {
    /* REFERENCE:
     *  LRIT/HRIT Global Specification (CGMS 03, Issue 2.6, 12.08.1999)
     */
    int ret = GRIB_SUCCESS;
    double *lats, *lons; /* arrays of latitudes and longitudes */
    double latOfSubSatellitePointInDegrees, lonOfSubSatellitePointInDegrees;
    double orientationInDegrees, nrInRadiusOfEarth;
    double radius = 0, xpInGridLengths = 0, ypInGridLengths = 0;
    long nx, ny, earthIsOblate                              = 0;
    long alternativeRowScanning, iScansNegatively;
    long Xo, Yo, jScansPositively, jPointsAreConsecutive, i;

    double major = 0, minor = 0, r_eq, r_pol, height;
    double lap, lop, angular_size;
    double xp, yp, dx, dy, rx, ry, x, y;
    double cos_x, cos_y, sin_x, sin_y;
    double factor_1, factor_2, tmp1, Sd, Sn, Sxy, S1, S2, S3;
    int x0, y0, ix, iy;
    double *s_x, *c_x; /* arrays storing sin and cos values */
    size_t array_size = (iter->nv * sizeof(double));

    grib_iterator_space_view* self = (grib_iterator_space_view*)iter;

    const char* sradius                          = grib_arguments_get_name(h, args, self->carg++);
    const char* sEarthIsOblate                   = grib_arguments_get_name(h, args, self->carg++);
    const char* sMajorAxisInMetres               = grib_arguments_get_name(h, args, self->carg++);
    const char* sMinorAxisInMetres               = grib_arguments_get_name(h, args, self->carg++);
    const char* snx                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sny                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sLatOfSubSatellitePointInDegrees = grib_arguments_get_name(h, args, self->carg++);
    const char* sLonOfSubSatellitePointInDegrees = grib_arguments_get_name(h, args, self->carg++);
    const char* sDx                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sDy                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sXpInGridLengths                 = grib_arguments_get_name(h, args, self->carg++);
    const char* sYpInGridLengths                 = grib_arguments_get_name(h, args, self->carg++);
    const char* sOrientationInDegrees            = grib_arguments_get_name(h, args, self->carg++);
    const char* sNrInRadiusOfEarthScaled         = grib_arguments_get_name(h, args, self->carg++);
    const char* sXo                              = grib_arguments_get_name(h, args, self->carg++);
    const char* sYo                              = grib_arguments_get_name(h, args, self->carg++);

    const char* siScansNegatively       = grib_arguments_get_name(h, args, self->carg++);
    const char* sjScansPositively       = grib_arguments_get_name(h, args, self->carg++);
    const char* sjPointsAreConsecutive  = grib_arguments_get_name(h, args, self->carg++);
    const char* sAlternativeRowScanning = grib_arguments_get_name(h, args, self->carg++);

    if ((ret = grib_get_long_internal(h, snx, &nx)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sny, &ny)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sEarthIsOblate, &earthIsOblate)) != GRIB_SUCCESS)
        return ret;

    if (earthIsOblate) {
        if ((ret = grib_get_double_internal(h, sMajorAxisInMetres, &major)) != GRIB_SUCCESS)
            return ret;
        if ((ret = grib_get_double_internal(h, sMinorAxisInMetres, &minor)) != GRIB_SUCCESS)
            return ret;
    }
    else {
        if ((ret = grib_get_double_internal(h, sradius, &radius)) != GRIB_SUCCESS)
            return ret;
    }

    if (iter->nv != nx * ny) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Wrong number of points (%zu!=%ldx%ld)", ITER, iter->nv, nx,
                         ny);
        return GRIB_WRONG_GRID;
    }
    if ((ret = grib_get_double_internal(h, sLatOfSubSatellitePointInDegrees, &latOfSubSatellitePointInDegrees))
        != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sLonOfSubSatellitePointInDegrees, &lonOfSubSatellitePointInDegrees))
        != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sDx, &dx)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sDy, &dy)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sXpInGridLengths, &xpInGridLengths)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sYpInGridLengths, &ypInGridLengths)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_double_internal(h, sOrientationInDegrees, &orientationInDegrees)) != GRIB_SUCCESS)
        return ret;

    /* Orthographic not supported. This happens when Nr (camera altitude) is missing */
    if (grib_is_missing(h, "Nr", &ret)) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Orthographic view (Nr missing) not supported", ITER);
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    if ((ret = grib_get_double_internal(h, sNrInRadiusOfEarthScaled, &nrInRadiusOfEarth)) != GRIB_SUCCESS)
        return ret;

    if ((ret = grib_get_long_internal(h, sXo, &Xo)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sYo, &Yo)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sjPointsAreConsecutive, &jPointsAreConsecutive)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sjScansPositively, &jScansPositively)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, siScansNegatively, &iScansNegatively)) != GRIB_SUCCESS)
        return ret;
    if ((ret = grib_get_long_internal(h, sAlternativeRowScanning, &alternativeRowScanning)) != GRIB_SUCCESS)
        return ret;

    if (earthIsOblate) {
        r_eq  = major; /* In km */
        r_pol = minor;
    }
    else {
        r_eq = r_pol = radius * 0.001; /*conv to km*/
    }

    if (nrInRadiusOfEarth == 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Key %s must be greater than zero", ITER,
                         sNrInRadiusOfEarthScaled);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    angular_size = 2.0 * asin(1.0 / nrInRadiusOfEarth);
    height       = nrInRadiusOfEarth * r_eq;

    lap = latOfSubSatellitePointInDegrees;
    lop = lonOfSubSatellitePointInDegrees;
    if (lap != 0.0) {
        grib_context_log(h->context, GRIB_LOG_ERROR,
                         "%s: Key %s must be 0 (satellite must be located in the equator plane)", ITER,
                         sLatOfSubSatellitePointInDegrees);
        return GRIB_GEOCALCULUS_PROBLEM;
    }

    /*orient_angle = orientationInDegrees;*/
    /* if (orient_angle != 0.0) return GRIB_NOT_IMPLEMENTED; */

    xp = xpInGridLengths;
    yp = ypInGridLengths;
    x0 = Xo;
    y0 = Yo;

    /* adjustBadlyEncodedEcmwfGribs(h, &nx, &ny, &dx, &dy, &xp, &yp); */
    if (dx == 0 || dy == 0) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Keys %s and %s must be greater than zero", ITER, sDx, sDy);
        return GRIB_GEOCALCULUS_PROBLEM;
    }
    rx = angular_size / dx;
    ry = (r_pol / r_eq) * angular_size / dy;

    self->lats = (double*)grib_context_malloc(h->context, array_size);
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, array_size);
        return GRIB_OUT_OF_MEMORY;
    }
    self->lons = (double*)grib_context_malloc(h->context, array_size);
    if (!self->lats) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, array_size);
        return GRIB_OUT_OF_MEMORY;
    }
    lats = self->lats;
    lons = self->lons;

    if (!iScansNegatively) {
        xp = xp - x0;
    }
    else {
        xp = (nx - 1) - (xp - x0);
    }
    if (jScansPositively) {
        yp = yp - y0;
    }
    else {
        yp = (ny - 1) - (yp - y0);
    }
    i        = 0;
    factor_2 = (r_eq / r_pol) * (r_eq / r_pol);
    factor_1 = height * height - r_eq * r_eq;

    /* Store array of sin and cosine values to avoid recalculation */
    s_x = (double*)grib_context_malloc(h->context, nx * sizeof(double));
    if (!s_x) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, nx * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }
    c_x = (double*)grib_context_malloc(h->context, nx * sizeof(double));
    if (!c_x) {
        grib_context_log(h->context, GRIB_LOG_ERROR, "%s: Error allocating %zu bytes", ITER, nx * sizeof(double));
        return GRIB_OUT_OF_MEMORY;
    }

    for (ix = 0; ix < nx; ix++) {
        x       = (ix - xp) * rx;
        s_x[ix] = sin(x);
        c_x[ix] = sqrt(1.0 - s_x[ix] * s_x[ix]);
    }

    /*for (iy = 0; iy < ny; iy++) {*/
    for (iy = ny - 1; iy >= 0; --iy) {
        y     = (iy - yp) * ry;
        sin_y = sin(y);
        cos_y = sqrt(1.0 - sin_y * sin_y);

        tmp1 = (1 + (factor_2 - 1.0) * sin_y * sin_y);

        for (ix = 0; ix < nx; ix++, i++) {
            /*x = (ix - xp) * rx;*/
            /* Use sin/cos previously computed */
            sin_x = s_x[ix];
            cos_x = c_x[ix];

            Sd = height * cos_x * cos_y;
            Sd = Sd * Sd - tmp1 * factor_1;
            if (Sd <= 0.0) {           /* outside of view */
                lats[i] = lons[i] = 0; /* TODO: error? */
            }
            else {
                Sd      = sqrt(Sd);
                Sn      = (height * cos_x * cos_y - Sd) / tmp1;
                S1      = height - Sn * cos_x * cos_y;
                S2      = Sn * sin_x * cos_y;
                S3      = Sn * sin_y;
                Sxy     = sqrt(S1 * S1 + S2 * S2);
                lons[i] = atan(S2 / S1) * (util::RADIAN_TO_DEGREE) + lop;
                lats[i] = atan(factor_2 * S3 / Sxy) * (util::RADIAN_TO_DEGREE);
                /*fprintf(stderr, "lat=%g   lon=%g\n", lats[i], lons[i]);*/
            }
            while (lons[i] < 0)
                lons[i] += 360;
            while (lons[i] > 360)
                lons[i] -= 360;
        }
    }
    grib_context_free(h->context, s_x);
    grib_context_free(h->context, c_x);
    iter->e = -1;

    return ret;
}
#endif


}  // namespace eckit::geo::projection
