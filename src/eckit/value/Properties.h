/*
 * (C) Copyright 1996- ECMWF.
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

#include "eckit/value/Params.h"
#include "eckit/value/Value.h"

namespace eckit {

class MD5;

//----------------------------------------------------------------------------------------------------------------------

class Properties {

public:  // types

    typedef Value property_t;
    typedef std::string key_t;

public:  // methods

    Properties();
    Properties(const property_t&);

    Properties(Stream&);

    virtual ~Properties() {}

    /// @returns true is a property exists
    bool has(const key_t&) const;

    /// @returns a property
    property_t get(const key_t& k) const;

    /// Sets a property by inserting a new or overwrites an existing property
    Properties& set(const key_t& k, const property_t& v);

    /// Sets a property by inserting a new or overwrites an existing property
    Properties& set(const key_t& k, const Properties& p);

    /// merge other properties
    Properties& set(const Properties& p);

    /// Removes a property
    bool remove(const key_t& k);

    /// @returns a property
    property_t operator[](const key_t& k) const { return get(k); }

    /// @returns a bool, true if empty false otherwise
    bool empty() const { return props_.empty(); }

    static const char* className() { return "eckit::Properties"; }

    operator property_t() const;

    void hash(eckit::MD5&) const;

protected:

    void print(std::ostream& s) const;

private:  // types

    typedef std::map<key_t, property_t> PropertyMap;

private:  // members

    PropertyMap props_;  //< storage of values

protected:  // methods

    void json(JSON& s) const;
    void encode(Stream& s) const;

    friend JSON& operator<<(JSON& s, const Properties& v) {
        v.json(s);
        return s;
    }
    friend std::ostream& operator<<(std::ostream& s, const Properties& v) {
        v.print(s);
        return s;
    }
    friend Stream& operator<<(Stream& s, const Properties& v) {
        v.encode(s);
        return s;
    }

    friend property_t getValue(const Properties& p, const key_t& key);
    friend void print(const Properties& p, std::ostream& s);
    friend void encode(const Properties& p, Stream& s);

    friend class Value;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
