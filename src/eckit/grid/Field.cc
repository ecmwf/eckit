/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"

#include "eckit/grid/Field.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace grid {

//-----------------------------------------------------------------------------

Field::MetaData::MetaData()
{
}

//-----------------------------------------------------------------------------

Field::Field(MetaData* metadata, std::vector<double>* data) :
    metadata_(metadata),
    data_(data)
{
    ASSERT(metadata_);
    ASSERT(data_);

    Log::info() << "Build a Field" << std::endl;
}

Field::~Field()
{
    if(metadata_) delete metadata_;
    if(data_) delete data_;
}

//-----------------------------------------------------------------------------

FieldSet::FieldSet(Grid *grid, const Field::Vector& fields) :
    grid_(grid),
    fields_(fields)
{
    ASSERT(grid_);
}

FieldSet::~FieldSet()
{
    for( size_t i = 0; i < fields_.size(); ++i )
        if(fields_[i]) delete fields_[i];
}

//-----------------------------------------------------------------------------

} // namespace grid
} // namespace eckit

