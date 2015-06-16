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
#include "eckit/value/Params.h"
#include "eckit/value/Properties.h"

namespace eckit {

//-----------------------------------------------------------------------------

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

bool Properties::get(const std::string& name, std::string& value) const
{
  if(!has(name)) return false;
  value = std::string(operator[](name));
  return true;
}
bool Properties::get(const std::string& name, bool& value) const
{
  if(!has(name)) return false;
  value = operator[](name);
  return true;
}
bool Properties::get(const std::string& name, long& value) const
{
  if(!has(name)) return false;
  value = operator[](name);
  return true;
}
bool Properties::get(const std::string& name, size_t& value) const
{
  if(!has(name)) return false;
  value = operator[](name);
  return true;
}
bool Properties::get(const std::string& name, double& value) const
{
  if(!has(name)) return false;
  value = operator[](name);
  return true;
}
bool Properties::get(const std::string& name, std::vector<int>& value) const
{
  if(!has(name)) return false;
  std::vector<eckit::Value> v = operator[](name);
  value.assign(v.begin(),v.end());
  return true;
}
bool Properties::get(const std::string& name, std::vector<long>& value) const
{
  if(!has(name)) return false;
  std::vector<eckit::Value> v = operator[](name);
  value.assign(v.begin(),v.end());
  return true;
}
bool Properties::get(const std::string& name, std::vector<double>& value) const
{
  if(!has(name)) return false;
  std::vector<eckit::Value> v = operator[](name);
  value.assign(v.begin(),v.end());
  return true;
}


Properties::property_t Properties::get(const key_t & k) const
{
    PropertyMap::const_iterator vit = props_.find(k);
    if( vit != props_.end() )
        return (*vit).second;
    else
        return property_t(); // return Nil Value...
}

Properties& Properties::set(const key_t & k, const property_t& v)
{
    props_[k] = v;
    return *this;
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

//-----------------------------------------------------------------------------

Properties::property_t getValue( const Properties& p, const Properties::key_t& key )
{
    return p.get(key);
}

void print( const Properties& p, std::ostream& s )
{
    s << p;
}

void encode( const Properties& p, Stream& s )
{
    s << p;
}

//-----------------------------------------------------------------------------

Params::Factory<Properties> propertiesFactory;

//-----------------------------------------------------------------------------

} // namespace eckit

