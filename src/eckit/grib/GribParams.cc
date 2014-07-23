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

namespace eckit {

//------------------------------------------------------------------------------------------------------

GribParams* GribParams::create( GribHandle::Ptr gh )
{
	return Factory<GribParams>::instance().get( gh->gridType() ).create(gh);
}

GribParams::GribParams(GribHandle::Ptr gh)
{
	set("gridType",gh->gridType());

	long edition = gh->edition();
	set("GRIB.edition", edition);

	/// @todo temporary until we use a better unique hash that works also with other formats
	set("hash", gh->geographyHash());

	set("GRIB.geographyHash", gh->geographyHash());

	north_ = gh->latitudeOfFirstGridPointInDegrees();
	south_ = gh->latitudeOfLastGridPointInDegrees();
	west_  = gh->longitudeOfFirstGridPointInDegrees();
	east_  = gh->longitudeOfLastGridPointInDegrees();

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

	set("nbDataPoints", gh->nbDataPoints() );
}

GribParams::~GribParams()
{
}

//------------------------------------------------------------------------------------------------------

class GribRegularLatLon : public GribParams {

public:

GribRegularLatLon( GribHandle::Ptr gh ) : GribParams(gh)
{
#if 0
   // Extract the regular lat long grid attributes from the grib handle

   GRIB_CHECK(grib_get_double(handle_,"jDirectionIncrementInDegrees",&(the_grid_->nsIncrement_)),0);
   GRIB_CHECK(grib_get_double(handle_,"iDirectionIncrementInDegrees",&(the_grid_->weIncrement_)),0);

   GRIB_CHECK(grib_get_long(handle_,"Nj",&(the_grid_->nptsNS_)),0);
   GRIB_CHECK(grib_get_long(handle_,"Ni",&(the_grid_->nptsWE_)),0);


   double plat = north_;
   the_grid_->points_.reserve( (the_grid_->nptsNS_ + 1) * (the_grid_->nptsWE_ + 1) );
   for( size_t j = 0; j < the_grid_->nptsNS_; ++j) {
	  double plon = west_;
	  for( size_t i = 0; i < the_grid_->nptsWE_; ++i) {
		 the_grid_->points_.push_back( Grid::Point( plat, plon ) );
		 plon += the_grid_->weIncrement_;
	  }
	  plat -= the_grid_->nsIncrement_;
   }

#ifdef DEBUG
   Log::info() << " editionNumber                                  " << editionNumber_ << std::endl;
   Log::info() << " iScansNegatively                               " << iScansNegatively_ << std::endl;
   Log::info() << " jScansPositively                               " << jScansPositively_ << std::endl;
   Log::info() << " scanning_mode                                  " << scanningMode(iScansNegatively_,jScansPositively_) << std::endl;
   Log::info() << " latitudeOfFirstGridPointInDegrees              " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << north_ << std::endl;
   Log::info() << " longitudeOfFirstGridPointInDegrees             " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << west_ << std::endl;
   Log::info() << " latitudeOfLastGridPointInDegrees               " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << south_ << std::endl;
   Log::info() << " longitudeOfLastGridPointInDegrees              " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << east_ << std::endl;
   Log::info() << " jDirectionIncrementInDegrees(north-south incr) " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << the_grid_->nsIncrement_ << std::endl;
   Log::info() << " iDirectionIncrementInDegrees(west-east   incr) " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << the_grid_->weIncrement_ << std::endl;
   Log::info() << " Nj(num of points North South)                  " << the_grid_->nptsNS_ << std::endl;
   Log::info() << " Ni(num of points West East)                    " << the_grid_->nptsWE_ << std::endl;
   Log::info() << " numberOfDataPoints                             " << numberOfDataPoints_ << std::endl;
   Log::info() << " -----------------------------------------------" << std::endl;
   Log::info() << " computeIncLat() " << the_grid_->computeIncLat() << "      nsIncrement_ " << the_grid_->nsIncrement_ << std::endl;
   Log::info() << " computeIncLon() " << the_grid_->computeIncLon() << "      weIncrement_ " << the_grid_->nsIncrement_ << std::endl;
   Log::info() << " computeRows()   " << the_grid_->computeRows(north_,south_,west_,east_) << "     nptsNS_ " << the_grid_->nptsNS_ << std::endl;
   Log::info() << " computeCols()   " << the_grid_->computeCols(west_,east_) <<  "     nptsWE_ " << the_grid_->nptsWE_ << std::endl;
   Log::info() << " points_.size()  " << the_grid_->points_.size() << "     numberOfDataPoints_ " << numberOfDataPoints_ << std::endl << std::endl;
#endif

   ASSERT(FloatCompare::is_equal(the_grid_->nsIncrement_,the_grid_->computeIncLat(),0.01));
   ASSERT(FloatCompare::is_equal(the_grid_->weIncrement_,the_grid_->computeIncLon(),0.01));
   ASSERT(the_grid_->nptsNS_ == the_grid_->computeRows(north_,south_,west_,east_));
   ASSERT(the_grid_->nptsWE_ == the_grid_->computeCols(west_,east_));
   ASSERT(the_grid_->points_.size() == numberOfDataPoints_);

#ifdef DEBUG
   // Check point list compared with grib
   comparePointList(the_grid_->points_,epsilon(),handle_);
#endif
#endif
}

};

ConcreteBuilderT1<GribParams,GribRegularLatLon> GribRegularLatLon_builder;

//------------------------------------------------------------------------------------------------------

} // namespace eckit
