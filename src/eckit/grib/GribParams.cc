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
	if (gh.hasKey("latitudeOfLastGridPointInDegrees")) {

	   double north = gh.latitudeOfFirstGridPointInDegrees();
	   double south = gh.latitudeOfLastGridPointInDegrees();
	   double west = gh.longitudeOfFirstGridPointInDegrees();
	   double east  = gh.longitudeOfLastGridPointInDegrees();

	   // ignore scanning mode:
	   north_ = std::max(north,south);
	   south_ = std::min(north,south);
	   east_ = std::max(east,west);
	   west_ = std::min(east,west);

	   set("grib_bbox_n", north_ );
	   set("grid_bbox_s", south_ );
	   set("grid_bbox_w", west_  );
	   set("grid_bbox_e", east_  );

	   // check area
	   ASSERT(north_ > south_); // This assertion only make sense if we ignore scanning mode
	   ASSERT(north_ < 90.0  || FloatCompare::is_equal(north_,90.0,degreesEps_));
	   ASSERT(south_ < 90.0  || FloatCompare::is_equal(south_,90.0,degreesEps_));
	   ASSERT(north_ > -90.0 || FloatCompare::is_equal(north_,-90.0,degreesEps_));
	   ASSERT(south_ > -90.0 || FloatCompare::is_equal(south_,-90.0,degreesEps_));

	   eckit::geometry::reduceTo2Pi(west_);
	   eckit::geometry::reduceTo2Pi(east_);

	   ASSERT(east_ > west_); // This assertion only make sense if we ignore scanning mode
	}

	no_of_data_points_ = gh.nbDataPoints();
	set("nbDataPoints", no_of_data_points_ );
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
		set( "GaussN", GribAccessor<long>("numberOfParallelsBetweenAPoleAndTheEquator")(gh) );

		set( "Nj", GribAccessor<long>("Nj")(gh) );

		/// @todo this may be optimized, maybe by using Value to fully wrap std::vector<long>

		std::vector<long> pl = GribAccessor< std::vector<long> >("pl")(gh);

		ValueList vpl(pl.size());
		for( size_t i = 0; i < pl.size(); ++i )
			vpl[i] = pl[i];

		set( "NPtsPerLat", vpl );
	}
};

ConcreteBuilderT1<GribParams,GribReducedGG> GribReducedGG_builder( "reduced_gg" );

//------------------------------------------------------------------------------------------------------

class GribRegularGG : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribRegularGG"; }
	GribRegularGG( GribHandle& gh ) : GribParams(gh)
	{
		set( "GaussN", GribAccessor<long>("numberOfParallelsBetweenAPoleAndTheEquator")(gh) );

		set( "Ni", GribAccessor<long>("Ni")(gh) );
	}

};

ConcreteBuilderT1<GribParams,GribRegularGG> GribRegularGG_builder( "regular_gg" );

//------------------------------------------------------------------------------------------------------

class GribRegularLatLon : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribRegularLatLon"; }
	GribRegularLatLon( GribHandle& gh ) : GribParams(gh)
	{
		set( "grid_lat_inc", GribAccessor<double>("jDirectionIncrementInDegrees")(gh) );
		set( "grid_lon_inc", GribAccessor<double>("iDirectionIncrementInDegrees")(gh) );

		set( "Nj", GribAccessor<long>("Nj")(gh) );
		set( "Ni", GribAccessor<long>("Ni")(gh) );
	}

};

ConcreteBuilderT1<GribParams,GribRegularLatLon> GribRegularLatLon_builder( "regular_ll" );

//------------------------------------------------------------------------------------------------------

class GribReducedLatLon : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribReducedLatLon"; }
	GribReducedLatLon( GribHandle& gh ) : GribParams(gh)
	{
		set( "grid_lat_inc", GribAccessor<double>("jDirectionIncrementInDegrees")(gh) );

		set( "Nj", GribAccessor<long>("Nj")(gh) );

      std::vector<long> pl = GribAccessor< std::vector<long> >("pl")(gh);
      ValueList vpl(pl.size());
      for( size_t i = 0; i < pl.size(); ++i )
         vpl[i] = pl[i];

      set( "NPtsPerLat", vpl );
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

      if (gh.hasKey("LaD")) set( "LaD", GribAccessor<double>("LaDInDegrees")(gh) );
      set( "orientationOfTheGrid", GribAccessor<double>("orientationOfTheGridInDegrees")(gh) );
      set( "latitudeOfFirstGridPoint", GribAccessor<double>("latitudeOfFirstGridPointInDegrees")(gh) );
      set( "longitudeOfFirstGridPoint", GribAccessor<double>("longitudeOfFirstGridPointInDegrees")(gh) );

      // Needed to determine bounding box
      set( "iScansPositively", GribAccessor<bool>("iScansPositively")(gh)  );
      set( "jScansPositively", GribAccessor<bool>("jScansPositively")(gh)  );

      set( "north_pole_on_projection_plane", !GribAccessor<bool>("southPoleOnProjectionPlane")(gh)  );
//      bool north_pole_on_projection_plane = true;
//      long projection_center_flag = GribAccessor<long>("projectionCentreFlag")(gh);
//      if (projection_center_flag == 1) north_pole_on_projection_plane =  false;
//      set( "north_pole_on_projection_plane", north_pole_on_projection_plane  );

      bool sphere = true;
      // TODO get sphere from grib ???
      set( "spherical_earth", eckit::Value(sphere) ); // true means sphere, false oblate spheroid

      if (sphere) {
         // get radius from grib ???
         // TODO
         double radius = 6371229.0 ;
         set( "radius", eckit::Value(radius) );
      }
      else {
         // TODO get radius from  semi_major and semi_minor from grib ???
         double semi_major = 6378137.0;
         double semi_minor = 6356752.3;
         set( "semi_major", eckit::Value(semi_major) );
         set( "semi_minor", eckit::Value(semi_minor) );
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
		set( "grid_lat_inc", GribAccessor<double>("jDirectionIncrementInDegrees")(gh) );
		set( "grid_lon_inc", GribAccessor<double>("iDirectionIncrementInDegrees")(gh) );

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
