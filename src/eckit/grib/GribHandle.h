#ifndef GribHandle_H
#define GribHandle_H
/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/Buffer.h"
#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
#include "eckit/serialisation/Stream.h"

struct grib_handle;

namespace eckit {

class DataHandle;

//------------------------------------------------------------------------------------------------------

class GribHandle : public eckit::Owned {
public:

    typedef eckit::SharedPtr<GribHandle> Ptr;

// -- Contructors

    GribHandle(grib_handle*);
    GribHandle(const eckit::Buffer&, bool copy = true);

// -- Destructor

	~GribHandle(); 

// -- Methods

    GribHandle* clone() const;

    size_t getDataValuesSize() const;

    double* getDataValues(size_t&) const;
    void getDataValues(double*, const size_t &) const;

    void setDataValues(const double*, size_t);
    void write(DataHandle&);

    /// @returns the raw grib_handle so client code can call grib directly. May return null.
    grib_handle* raw() { return handle_; }

private: // members

    grib_handle* handle_;

};

//------------------------------------------------------------------------------------------------------

}

#endif
