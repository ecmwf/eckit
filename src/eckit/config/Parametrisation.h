/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Apr 2015


#ifndef eckit_config_Parametrisation_H
#define eckit_config_Parametrisation_H

#include <string>
#include <vector>

namespace eckit {


class Parametrisation {
  public:

// -- Exceptions
    // None

// -- Contructors


// -- Destructor

// -- Convertors
    // None

// -- Operators
    // None

// -- Methods
    // None

    virtual bool has(const std::string& name) const = 0;

    virtual bool get(const std::string& name, std::string& value) const = 0;
    virtual bool get(const std::string& name, bool& value) const = 0;
    virtual bool get(const std::string& name, int& value) const;
    virtual bool get(const std::string& name, long& value) const = 0;
    virtual bool get(const std::string& name, size_t& value) const = 0;
    virtual bool get(const std::string& name, float& value) const;
    virtual bool get(const std::string& name, double& value) const = 0;

    virtual bool get(const std::string& name, std::vector<int>& value) const;
    virtual bool get(const std::string& name, std::vector<long>& value) const = 0;
    virtual bool get(const std::string& name, std::vector<float>& value) const;
    virtual bool get(const std::string& name, std::vector<double>& value) const = 0;


// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  protected:

// -- Members
    // None

// -- Methods


// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

  private:

// No copy allowed

// -- Members
    // None

// -- Methods
    // None

// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends


};


inline bool Parametrisation::get(const std::string& name, int& value) const
{
  long v;
  if( get(name,v) )
  {
    value = v;
    return true;
  }
  return false;
}

inline bool Parametrisation::get(const std::string& name, float& value) const
{
  double v;
  if( get(name,v) )
  {
    value = v;
    return true;
  }
  return false;
}

inline bool Parametrisation::get(const std::string& name, std::vector<int>& value) const
{
  std::vector<long> v;
  if( get(name,v) )
  {
    value.assign(v.begin(),v.end());
    return true;
  }
  return false;
}

inline bool Parametrisation::get(const std::string& name, std::vector<float>& value) const
{
  std::vector<float> v;
  if( get(name,v) )
  {
    value.assign(v.begin(),v.end());
    return true;
  }
  return false;
}

}  // namespace eckit
#endif

