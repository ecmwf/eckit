/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Peter Bispham
/// @author Tiago Quintino
/// @date Oct 2013

#ifndef eckit_grid_Field_H
#define eckit_grid_Field_H

#include <vector>

#include "eckit/types/Types.h"
#include "eckit/memory/NonCopyable.h"
#include "Grid.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace grid {


//-----------------------------------------------------------------------------

/// Represents a Field which store one type of data parameter
class Field : private eckit::NonCopyable {
public: // types

    class MetaData : public eckit::StringDict {
    public: // methods

        MetaData();

    };

    typedef std::vector< Field* >  Vector;
    typedef std::vector< Field* >::const_iterator Iterator;
    typedef std::vector< double >  Data;

public: // methods

    Field( MetaData* metadata, std::vector<double>* data );

    ~Field();

    MetaData* metadata() const { return metadata_; }
    Data*     data() const { return data_; }

protected:

    MetaData*   metadata_;  ///< describes the field
    Data*       data_;      ///< stores the field data

};

//-----------------------------------------------------------------------------

/// Represents a set of fields
/// @todo add an iterator class to this collection
class FieldSet : private eckit::NonCopyable {

public: // methods

    /// Takes ownership of the fields
    FieldSet( Grid* grid, const Field::Vector& fields = Field::Vector() );

    ~FieldSet();

    // passes ownership to the caller
    FieldSet* operator() (size_t i, size_t j, size_t k = 1); ///@todo implement

    // helps iterate over the fields of this FieldSet
    //Field* next(); /// @todo implement
    
    /// @todo revise
    const Field::Vector& fields() const { return fields_; }
    Field::Vector& fields() { return fields_; }


    //GridPtr grid() const { return GridPtr(grid_); }
    Grid* grid() const { return grid_; }
    
protected:

    /// @todo: make this a shared pointer
    Grid* grid_;

    // It is important that the order of the fields is respected
    Field::Vector fields_;
};

//-----------------------------------------------------------------------------

} // namespace grid
} // namespace eckit

#endif
