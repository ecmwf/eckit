/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_grib_GribParams_H
#define eckit_grib_GribParams_H

#include "eckit/grib/GribHandle.h"
#include "eckit/memory/Builder.h"
#include "eckit/value/Params.h"

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

class GribParams : public eckit::ValueParams {
public:

	typedef BuilderT1<GribParams> builder_t;
	typedef GribHandle&           ARG1;

	static std::string className() { return "eckit.grib.GribParams"; }

	GribParams( GribHandle& );

	virtual ~GribParams();

	static GribParams* create( GribHandle& );

protected: // members

	GribHandle& g_;

	long   edition_;                 ///< Grib 1 or Grib 2

	double north_;                   ///< In degrees
	double south_;                   ///< In degrees
	double west_;                    ///< In degrees
	double east_;                    ///< In degrees
	double degreesEps_;              ///< epsilon for comparing degrees

	std::string hash_;               ///< may be used to persist grids
};

//------------------------------------------------------------------------------------------------------

}
}

#endif
