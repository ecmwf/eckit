/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/memory/Factory.h"
#include "eckit/memory/Builder.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/geometry/Point2.h"

#include "eckit/grib/GribParams.h"
#include "eckit/grib/GribAccessor.h"

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

GribParams* GribParams::create( GribHandle& gh )
{
	return Factory<GribParams>::instance().get( gh.gridType() ).create(gh);
}

GribParams::GribParams(GribHandle& gh)
: g_(gh),
  edition_(0),
  no_of_data_points_(0),
  north_(0),
  south_(0),
  west_(0),
  east_(0),
  degreesEps_(0)
{
	set("grid_type",gh.gridType());

	edition_ = gh.edition();
	set("GRIB.edition", edition_);

	if( edition_ == 1)
		set("DegreesEpsilon", 1E-3);
	else
		set("DegreesEpsilon", 1E-6);

   degreesEps_ = (edition_ == 1) ? 1e-3 : 1e-6; // GRIB1 is in mili while GRIB2 is in micro degrees

	/// @todo temporary until we use a better unique hash that works also with other formats
	std::string the_hash = gh.geographyHash();
	set("hash", the_hash);

	set("GRIB.geographyHash", the_hash);

	// Not all GRID's have a bounding box, i.e Polar Stereographic
	if (gh.hasKey("latitudeOfLastGridPointInDegrees"))
	{
	   double north = gh.latitudeOfFirstGridPointInDegrees();
	   double south = gh.latitudeOfLastGridPointInDegrees();
	   double west  = gh.longitudeOfFirstGridPointInDegrees();
	   double east  = gh.longitudeOfLastGridPointInDegrees();

	   eckit::geometry::reduceTo2Pi(west);
	   eckit::geometry::reduceTo2Pi(east);

	   // ignore scanning mode:
	   north_ = std::max(north,south);
	   south_ = std::min(north,south);
	   east_ = std::max(east,west);
	   west_ = std::min(east,west);

	   set("bbox_n", north_ );
	   set("bbox_s", south_ );
	   set("bbox_w", west_  );
	   set("bbox_e", east_  );

	   // check area
	   ASSERT(north_ > south_); // This assertion only make sense if we ignore scanning mode
	   ASSERT(north_ < 90.0  || FloatCompare::is_equal(north_,90.0,degreesEps_));
	   ASSERT(south_ < 90.0  || FloatCompare::is_equal(south_,90.0,degreesEps_));
	   ASSERT(north_ > -90.0 || FloatCompare::is_equal(north_,-90.0,degreesEps_));
	   ASSERT(south_ > -90.0 || FloatCompare::is_equal(south_,-90.0,degreesEps_));

	   ASSERT(east_ > west_); // This assertion only make sense if we ignore scanning mode
	}

	no_of_data_points_ = gh.npts();
	set("npts", no_of_data_points_ );
}

GribParams::~GribParams()
{
}

//------------------------------------------------------------------------------------------------------

class GribReducedGG : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribReducedGG"; }

	GribReducedGG( GribHandle& gh ) : GribParams(gh)
	{
		set( "N", GribAccessor<long>("numberOfParallelsBetweenAPoleAndTheEquator")(gh) );

		set( "Nj", GribAccessor<long>("Nj")(gh) );

		/// @todo this may be optimized, maybe by using Value to fully wrap std::vector<long>

		std::vector<long> pl = GribAccessor< std::vector<long> >("pl")(gh);

		ValueList vpl(pl.size());
		for( size_t i = 0; i < pl.size(); ++i )
			vpl[i] = pl[i];

		set( "npts_per_lat", vpl );
	}
};

ConcreteBuilderT1<GribParams,GribReducedGG> GribReducedGG_builder( "reduced_gg" );

//------------------------------------------------------------------------------------------------------

class GribRegularGG : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribRegularGG"; }
	GribRegularGG( GribHandle& gh ) : GribParams(gh)
	{
		set( "N", GribAccessor<long>("numberOfParallelsBetweenAPoleAndTheEquator")(gh) );

		set( "nlon", GribAccessor<long>("Ni")(gh) );
	}

};

ConcreteBuilderT1<GribParams,GribRegularGG> GribRegularGG_builder( "regular_gg" );

//------------------------------------------------------------------------------------------------------

class GribRegularLatLon : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribRegularLatLon"; }
	GribRegularLatLon( GribHandle& gh ) : GribParams(gh)
	{
		set( "lat_inc", GribAccessor<double>("jDirectionIncrementInDegrees")(gh) );
		set( "lon_inc", GribAccessor<double>("iDirectionIncrementInDegrees")(gh) );

		set( "nlon", GribAccessor<long>("Ni")(gh) );
		set( "nlat", GribAccessor<long>("Nj")(gh) );
  }

};

ConcreteBuilderT1<GribParams,GribRegularLatLon> GribRegularLatLon_builder( "regular_ll" );

//------------------------------------------------------------------------------------------------------

class GribReducedLatLon : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribReducedLatLon"; }
	GribReducedLatLon( GribHandle& gh ) : GribParams(gh)
	{
    set( "lat_inc", GribAccessor<double>("jDirectionIncrementInDegrees")(gh) );

    set( "nlat", GribAccessor<long>("Nj")(gh) );

    std::vector<long> pl = GribAccessor< std::vector<long> >("pl")(gh);
    ValueList vpl(pl.size());
    for( size_t i = 0; i < pl.size(); ++i )
    {
       vpl[i] = pl[i];
    }
    set( "npts_per_lat", vpl );

    // ReducedLatLon is a global grid. The "poles" variable notifies that
    // the poles are included in the grid
    set( "poles", true );
  }

};

ConcreteBuilderT1<GribParams,GribReducedLatLon> GribReducedLatLon_builder( "reduced_ll" );

//------------------------------------------------------------------------------------------------------

class GribPolarStereoGraphic : public GribParams {
public:

   static std::string className() { return "eckit.grib.GribPolarStereoGraphic"; }
   GribPolarStereoGraphic( GribHandle& gh ) : GribParams(gh)
   {
      long nx = GribAccessor<long>("Nx")(gh);
      long ny = GribAccessor<long>("Ny")(gh);
      ASSERT(no_of_data_points_ ==  nx*ny);
      set( "Nx", nx );
      set( "Ny", ny );

      set( "Dx", GribAccessor<long>("DxInMetres")(gh) );
      set( "Dy", GribAccessor<long>("DyInMetres")(gh) );

      set( "resolutionAndComponentFlag", GribAccessor<long>("resolutionAndComponentFlag")(gh) );

      if (gh.hasKey("LaD")) set( "LaD", GribAccessor<double>("LaDInDegrees")(gh) );
      set( "orientationOfTheGrid", GribAccessor<double>("orientationOfTheGridInDegrees")(gh) );
      set( "latitudeOfFirstGridPoint", GribAccessor<double>("latitudeOfFirstGridPointInDegrees")(gh) );
      set( "longitudeOfFirstGridPoint", GribAccessor<double>("longitudeOfFirstGridPointInDegrees")(gh) );

      // Needed to determine bounding box
      set( "iScansPositively", GribAccessor<bool>("iScansPositively")(gh)  );
      set( "jScansPositively", GribAccessor<bool>("jScansPositively")(gh)  );

      set( "southPoleOnProjectionPlane", GribAccessor<bool>("southPoleOnProjectionPlane")(gh) );

      bool earthIsOblate = GribAccessor<bool>("earthIsOblate")(gh);
      set( "earthIsOblate", eckit::Value(earthIsOblate) ); // true means sphere, false oblate spheroid

      if (earthIsOblate) {
         set( "earthMajorAxis", GribAccessor<double>("earthMajorAxisInMetres")(gh) );
         set( "earthMinorAxis", GribAccessor<double>("earthMinorAxisInMetres")(gh) );
      }
      else {
         set( "radius", GribAccessor<double>("radius")(gh) );
      }
   }
};

ConcreteBuilderT1<GribParams,GribPolarStereoGraphic> GribPolarStereoGraphic_builder( "polar_stereographic" );


//------------------------------------------------------------------------------------------------------

class GribRotatedLatLon : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribRotatedLatLon"; }
	GribRotatedLatLon( GribHandle& gh ) : GribParams(gh)
	{
		set( "lat_inc", GribAccessor<double>("jDirectionIncrementInDegrees")(gh) );
		set( "lon_inc", GribAccessor<double>("iDirectionIncrementInDegrees")(gh) );

		set( "Nj", GribAccessor<long>("Nj")(gh) );
		set( "Ni", GribAccessor<long>("Ni")(gh) );

		set( "SouthPoleLat", GribAccessor<double>("latitudeOfSouthernPoleInDegrees")(gh) );
		set( "SouthPoleLon", GribAccessor<double>("longitudeOfSouthernPoleInDegrees")(gh) );
		set( "SouthPoleRotAngle", GribAccessor<double>("angleOfRotation")(gh) );
	}

};

ConcreteBuilderT1<GribParams,GribRotatedLatLon> GribRotatedLatLon_builder( "rotated_ll" );

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit
