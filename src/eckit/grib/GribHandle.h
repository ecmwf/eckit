/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_grib_GribHandle_H
#define eckit_grib_GribHandle_H

#include "eckit/io/Buffer.h"
#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
#include "eckit/serialisation/Stream.h"

struct grib_handle;

namespace eckit {

class DataHandle;

namespace geometry { class LLPoint2; }

namespace grib {

//------------------------------------------------------------------------------------------------------

int grib_call( int code, const char* msg );

#define GRIB_CALL(a) eckit::grib::grib_call(a, #a)

//------------------------------------------------------------------------------------------------------

class GribHandle : public eckit::Owned {

public: // types

    typedef eckit::SharedPtr<GribHandle> Ptr;

	/// cosntructor taking ownership of a grib_handle pointer
    GribHandle(grib_handle*);

	/// cosntructor not taking ownership of the grib_handle
	GribHandle(grib_handle&);

	/// cosntructor creating a grib_handle from a passed buffer
	GribHandle(const eckit::Buffer&, bool copy = true);

	~GribHandle();

public: // methods

	std::string gridType() const;

	std::string geographyHash() const;

    long edition() const;

    GribHandle* clone() const;

	size_t nbDataPoints() const;

    size_t getDataValuesSize() const;

    double* getDataValues(size_t&) const;
    void getDataValues(double*, const size_t &) const;

    void setDataValues(const double*, size_t);

    void write( DataHandle& );
    size_t write( Buffer& );

	double latitudeOfFirstGridPointInDegrees()  const;
	double longitudeOfFirstGridPointInDegrees() const;
	double latitudeOfLastGridPointInDegrees()   const;
	double longitudeOfLastGridPointInDegrees()  const;

	/// @returns the raw grib_handle so client code can call grib directly
    grib_handle* raw() { return handle_; }

private: // members

	bool owns_handle_;
	grib_handle* handle_; ///< owned grib_handle

};

//------------------------------------------------------------------------------------------------------

void read_latlon_from_grib(GribHandle& h, std::vector<geometry::LLPoint2>& points);

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit

#endif
