/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef GribHandle_H
#define GribHandle_H

#include "eckit/io/Buffer.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/serialisation/Stream.h"

class DataHandle;

struct grib_handle;
namespace eckit {

class GribHandle : private eckit::NonCopyable {
public:

// -- Contructors

    GribHandle(grib_handle*);
    GribHandle(const eckit::Buffer&,size_t, bool copy = true);

// -- Destructor

	~GribHandle(); 

// -- Methods

    GribHandle* clone() const;

    double* getDataValues(size_t&) const;
    void setDataValues(const double*, size_t);
    void write(DataHandle&);

private: // members

    grib_handle* handle_;

};

}

#endif
