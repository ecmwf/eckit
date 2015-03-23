/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/parser/JSON.h"
#include "eckit/types/Types.h"
#include "eckit/value/Properties.h"

namespace eckit {

//------------------------------------------------------------------------------------------------------

Properties::Properties()
{
}

Properties::Properties(Stream &s)
{
    s >> props_;
}

bool Properties::has(const key_t & k) const
{
    return ( props_.find(k) != props_.end() );
}

Properties::property_t Properties::get(const key_t & k) const
{
    PropertyMap::const_iterator vit = props_.find(k);
    if( vit != props_.end() )
        return (*vit).second;
    else
        return property_t(); // return Nil Value...
}

void Properties::set(const key_t & k, const property_t& v)
{
    props_[k] = v;
}

bool Properties::remove(const key_t & k)
{
    return props_.erase(k);
}

void Properties::json( JSON& s ) const
{
    s << props_;
}

void Properties::print( std::ostream& s ) const
{
    for( PropertyMap::const_iterator vit = props_.begin(); vit != props_.end(); ++vit )
        s << "(" << (*vit).first << "," << (*vit).second << ")";
}

void Properties::encode( Stream& s ) const
{
    s << props_;
}

//------------------------------------------------------------------------------------------------------

} // namespace eckit

