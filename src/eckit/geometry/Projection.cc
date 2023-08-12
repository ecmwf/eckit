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


#include "eckit/geometry/Projection.h"

#include <map>
#include <ostream>

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"


namespace eckit::geometry {


#if 0
- mercator:
    double DiInMetres
    double DjInMetres
    double LaDInDegrees
    double latitudeOfFirstPointInDegrees
    double longitudeOfFirstPointInDegrees
    double projectionOrientationInDegrees
    size_t n_i
    size_t n_j
- polar_stereographic:
    double DiInMetres
    double DjInMetres
    double LaDInDegrees
    double latitudeOfFirstPointInDegrees
    double longitudeOfFirstPointInDegrees
    double projectionOrientationInDegrees
    flags projectionCentreFlag
    size_t n_i
    size_t n_j
- lambert:
    double DiInMetres
    double DjInMetres
    double LaDInDegrees
    double Latin1InDegrees
    double Latin2InDegrees
    double LoVInDegrees
    double latitudeOfFirstPointInDegrees
    double latitudeOfSouthernPoleInDegrees
    double longitudeOfFirstPointInDegrees
    double longitudeOfSouthernPoleInDegrees
    flags projectionCentreFlag
    size_t n_i
    size_t n_j
- albers:
    double DiInMetres
    double DjInMetres
    double LaDInDegrees
    double Latin1InDegrees
    double Latin2InDegrees
    double LoVInDegrees
    double latitudeOfFirstPointInDegrees
    double latitudeOfSouthernPoleInDegrees
    double longitudeOfFirstPointInDegrees
    double longitudeOfSouthernPoleInDegrees
    flags projectionCentreFlag
    size_t n_i
    size_t n_j
- space_view:
    double DiInMetres
    double DjInMetres
    double altitudeFromEarthCentreInMetres
    double latitudeOfSubSatellitePointInDegrees
    double longitudeOfSubSatellitePointInDegrees
    double projectionOrientationInDegrees
    double xOriginOfSectorImageInMetres
    double xSubSatellitePointInMetres
    double yOriginOfSectorImageInMetres
    double ySubSatellitePointInMetres
    size_t n_i
    size_t n_j
- lambert_azimuthal_equal_area:
    double DiInMetres
    double DjInMetres
    double centralLongitudeInDegrees
    double latitudeOfFirstPointInDegrees
    double longitudeOfFirstPointInDegrees
    double standardParallelInDegrees
- equatorial_azimuthal_equidistant:
    double DiInMetres
    double DjInMetres
    double latitudeOfTangencyPointInDegrees
    double longitudeOfTangencyPointInDegrees
    flags projectionCentreFlag
- unstructured_grid (grib2/localConcepts/[centre:s]/unstructuredGrid*):
    string unstructuredGridSubtype = undefined/T/U/V/W/F (numberOfGridInReference)
    string unstructuredGridType = undefined/ORCA2/ORCA1/ORCA025/ORCA12/eORCA1/eORCA025/eORCA12 (numberOfGridInReference)
    string uuidOfHGrid
- irregular_latlon


// to instantiate ecCodes iterators
- regular_gg:
  - latitudeFirstInDegrees, longitudeFirstInDegrees
  - latitudeLastInDegrees
  - DiInDegrees
  - Ni
  - Nj
  - N
  - iScansNegatively, jScansPositively
- reduced_gg:
  - latitudeOfFirstGridPointInDegrees, longitudeOfFirstGridPointInDegrees
  - latitudeOfLastGridPointInDegrees, longitudeOfLastGridPointInDegrees
  - N
  - pl
  - Nj
- lambert_azimuthal_equal_area:
  - latitudeOfFirstGridPointInDegrees, longitudeOfFirstGridPointInDegrees
  - radius
  - Nx
  - Ny
  - standardParallelInDegrees
  - centralLongitudeInDegrees
  - Dx
  - Dy
  - iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning
- lambert:
  - latitudeOfFirstGridPointInDegrees, longitudeOfFirstGridPointInDegrees
  - radius
  - Nx
  - Ny
  - LoVInDegrees
  - LaDInDegrees
  - Latin1InDegrees
  - Latin2InDegrees
  - DxInMetres
  - DyInMetres
  - iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning
- regular_ll, rotated_ll:
  - latitudeFirstInDegrees, longitudeFirstInDegrees
  - DiInDegrees
  - Ni
  - Nj
  - DjInDegrees
  - isRotatedGrid
  - angleOfRotation, latitudeOfSouthernPoleInDegrees, longitudeOfSouthernPoleInDegrees
  - iScansNegatively, jScansPositively, jPointsAreConsecutive
- reduced_ll:
  - latitudeFirstInDegrees, longitudeFirstInDegrees
  - latitudeLastInDegrees, longitudeLastInDegrees
  - Nj
  - DjInDegrees
  - pl
- mercator:
  - latitudeOfFirstGridPointInDegrees, longitudeOfFirstGridPointInDegrees
  - latitudeOfLastGridPointInDegrees, longitudeOfLastGridPointInDegrees
  - radius
  - Ni
  - Nj
  - LaDInDegrees
  - orientationOfTheGridInDegrees
  - DiInMetres
  - DjInMetres
  - iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning
- polar_stereographic:
  - latitudeOfFirstGridPointInDegrees, longitudeOfFirstGridPointInDegrees
  - radius
  - Nx
  - Ny
  - southPoleOnProjectionPlane
  - orientationOfTheGridInDegrees
  - LaDInDegrees
  - DxInMetres
  - DyInMetres
  - iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning
- space_view:
  - radius
  - earthIsOblate
  - earthMajorAxis
  - earthMinorAxis
  - Nx
  - Ny
  - latitudeOfSubSatellitePointInDegrees
  - longitudeOfSubSatellitePointInDegrees
  - dx
  - dy
  - XpInGridLengths
  - YpInGridLengths
  - orientationOfTheGridInDegrees
  - NrInRadiusOfEarthScaled
  - Xo
  - Yo
  - iScansNegatively, jScansPositively, jPointsAreConsecutive, alternativeRowScanning


Projection, rotation, shape (grib2/template.3.(rotation|shape_of_the_earth).def)
- projSourceString, projTargetString, projString=projTargetString

    double latitudeOfSouthernPoleInDegrees
    double longitudeOfSouthernPoleInDegrees
    double angleOfRotationInDegrees

    bool is_oblate
    double majorAxisInMetres
    double minorAxisInMetres
    double radiusInMetres

Projection centre (grib2/tables/30/3.5.table)
  1 0 North Pole is on the projection plane
  1 1 South Pole is on the projection plane
  2 0 Only one projection centre is used
  2 1 Projection is bipolar and symmetric
#endif


static pthread_once_t __once                                       = PTHREAD_ONCE_INIT;
static Mutex* __mutex                                              = nullptr;
static std::map<Projection::Type, ProjectionFactory*>* __factories = nullptr;


static void __init() {
    __mutex     = new Mutex;
    __factories = new std::map<Projection::Type, ProjectionFactory*>();
}


Projection* ProjectionFactory::build(const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    Projection::Type type;
    ASSERT(config.get("type", type));

    return build(type, config);
}


Projection* ProjectionFactory::build(const Projection::Type& type,
                                     const Configuration& config) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto f = __factories->find(type); f != __factories->end()) {
        return f->second->make(config);
    }

    list(Log::error() << "ProjectionFactory: unknown '" << type << "', choices are: ");
    throw BadValue("ProjectionFactory: unknown '" + type + "'");
}


std::ostream& ProjectionFactory::list(std::ostream& out) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    JSON j(out);
    j.startObject();

    j << "type";
    j.startList();
    for (const auto& p : *__factories) {
        j << p.first;
    }
    j.endList();

    j.endObject();

    return out;
}


ProjectionFactory::ProjectionFactory(const Projection::Type& key) :
    key_(key) {
    pthread_once(&__once, __init);
    AutoLock<Mutex> lock(*__mutex);

    if (auto f = __factories->find(key); f != __factories->end()) {
        throw BadValue("ProjectionFactory: duplicate '" + key + "'");
    }

    (*__factories)[key] = this;
}


ProjectionFactory::~ProjectionFactory() {
    AutoLock<Mutex> lock(*__mutex);

    if (__factories != nullptr) {
        __factories->erase(key_);
    }
}


}  // namespace eckit::geometry
