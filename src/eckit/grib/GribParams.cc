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

GribParams::GribParams(GribHandle& gh) : g_(gh)
{
	set("grid_type",gh.gridType());

	long edition = gh.edition();
	set("GRIB.edition", edition);

	if( edition == 1)
		set("DegreesEpsilon", 1E-3);
	else
		set("DegreesEpsilon", 1E-6);

	/// @todo temporary until we use a better unique hash that works also with other formats
	set("hash", gh.geographyHash());

	set("GRIB.geographyHash", gh.geographyHash());

	north_ = gh.latitudeOfFirstGridPointInDegrees();
	south_ = gh.latitudeOfLastGridPointInDegrees();
	west_  = gh.longitudeOfFirstGridPointInDegrees();
	east_  = gh.longitudeOfLastGridPointInDegrees();

	// check area
	epsilon_ = (edition_ == 1) ? 1e-3 : 1e-6; // GRIB1 is in mili while GRIB2 is in micro degrees

	ASSERT(north_ > south_);
	ASSERT(north_ < 90.0  || FloatCompare::is_equal(north_,90.0,epsilon_));
	ASSERT(south_ < 90.0  || FloatCompare::is_equal(south_,90.0,epsilon_));
	ASSERT(north_ > -90.0 || FloatCompare::is_equal(north_,-90.0,epsilon_));
	ASSERT(south_ > -90.0 || FloatCompare::is_equal(south_,-90.0,epsilon_));

	eckit::geometry::reduceTo2Pi(west_);
	eckit::geometry::reduceTo2Pi(east_);

	ASSERT(east_ > west_);

	set("area_n", north_ );
	set("area_s", south_ );
	set("area_w", west_  );
	set("area_e", east_  );

	set("nbDataPoints", gh.nbDataPoints() );
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
		set( "GaussN", GribAccessor<long>("numberOfParallelsBetweenAPoleAndTheEquator")(gh.raw()) );

		set( "Nj", GribAccessor<long>("Nj")(gh.raw()) );

		/// @todo this may be optimized, maybe by using Value to fully wrap std::vector<long>

		std::vector<long> pl = GribAccessor< std::vector<long> >("pl")(gh.raw());

		ValueList vpl(pl.size());
		for( size_t i = 0; i < pl.size(); ++i )
			vpl[i] = pl[i];

		set( "NLats", vpl );

	}

};

ConcreteBuilderT1<GribParams,GribReducedGG> GribReducedGG_builder( "reduced_gg" );

//------------------------------------------------------------------------------------------------------

class GribRegularGG : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribRegularGG"; }
	GribRegularGG( GribHandle& gh ) : GribParams(gh)
	{
		set( "GaussN", GribAccessor<long>("numberOfParallelsBetweenAPoleAndTheEquator")(gh.raw()) );

		set( "Nj", GribAccessor<long>("Nj")(gh.raw()) );
	}

};

ConcreteBuilderT1<GribParams,GribRegularGG> GribRegularGG_builder( "regular_gg" );

//------------------------------------------------------------------------------------------------------

class GribRegularLatLon : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribRegularLatLon"; }
	GribRegularLatLon( GribHandle& gh ) : GribParams(gh)
	{
		set( "grid_ns_inc", GribAccessor<double>("jDirectionIncrementInDegrees")(gh.raw()) );
		set( "grid_ew_inc", GribAccessor<double>("iDirectionIncrementInDegrees")(gh.raw()) );

		set( "Nj", GribAccessor<long>("Nj")(gh.raw()) );
		set( "Ni", GribAccessor<long>("Ni")(gh.raw()) );
	}

};

ConcreteBuilderT1<GribParams,GribRegularLatLon> GribRegularLatLon_builder( "regular_ll" );

//------------------------------------------------------------------------------------------------------

class GribReducedLatLon : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribReducedLatLon"; }
	GribReducedLatLon( GribHandle& gh ) : GribParams(gh)
	{
		set( "grid_ns_inc", GribAccessor<double>("jDirectionIncrementInDegrees")(gh.raw()) );

		set( "Nj", GribAccessor<long>("Nj")(gh.raw()) );
	}

};

ConcreteBuilderT1<GribParams,GribReducedLatLon> GribReducedLatLon_builder( "reduced_ll" );

//------------------------------------------------------------------------------------------------------

class GribRotatedLatLon : public GribParams {
public:

	static std::string className() { return "eckit.grib.GribRotatedLatLon"; }
	GribRotatedLatLon( GribHandle& gh ) : GribParams(gh)
	{
		set( "grid_ns_inc", GribAccessor<double>("jDirectionIncrementInDegrees")(gh.raw()) );
		set( "grid_ew_inc", GribAccessor<double>("iDirectionIncrementInDegrees")(gh.raw()) );

		set( "Nj", GribAccessor<long>("Nj")(gh.raw()) );
		set( "Ni", GribAccessor<long>("Ni")(gh.raw()) );

		set( "SouthPoleLat", GribAccessor<double>("latitudeOfSouthernPoleInDegrees")(gh.raw()) );
		set( "SouthPoleLon", GribAccessor<double>("longitudeOfSouthernPoleInDegrees")(gh.raw()) );
		set( "SouthPoleRotAngle", GribAccessor<double>("angleOfRotation")(gh.raw()) );
	}

};

ConcreteBuilderT1<GribParams,GribRotatedLatLon> GribRotatedLatLon_builder( "rotated_ll" );

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit
