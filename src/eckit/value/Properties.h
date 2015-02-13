/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Properties.h
/// @author Tiago Quintino
/// @date Jun 2014

#ifndef eckit_Properties_h
#define eckit_Properties_h

#include <map>
#include <string>

#include "eckit/value/Value.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {

//------------------------------------------------------------------------------------------------------

class Properties {

public: // types

    typedef Value property_t;

public: // methods

    Properties();

    Properties(Stream&);

    /// @returns true is a property exists
    bool has( const std::string& k ) const;

    /// @returns a property
    property_t get( const std::string& k ) const;

    /// Sets a property by inserting a new or overwrites an existing property
    void set( const std::string& k, const property_t& v );

    /// Removes a property
    bool remove( const std::string& k );

	/// @returns a property
	property_t operator[]( const std::string& k ) const { return get(k); }

	/// @returns a bool, true if empty false otherwise
	bool empty() const { return props_.empty(); }

protected:
    void print(std::ostream& s) const;

private: // types

    typedef std::map< std::string, property_t > PropertyMap;

private: // members

    PropertyMap  props_; //< storage of values

private: // methods

    void json(JSON& s) const;
    void encode(Stream& s) const;

    friend JSON& operator<<(JSON& s, const Properties& v) { v.json(s);  return s; }
    friend std::ostream& operator<<(std::ostream& s, const Properties& v) { v.print(s);  return s; }
    friend Stream&  operator<<(Stream&  s, const Properties& v) { v.encode(s); return s; }

    friend class Value;
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
