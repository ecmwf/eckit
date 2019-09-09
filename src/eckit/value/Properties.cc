/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/value/Properties.h"
#include "eckit/log/JSON.h"
#include "eckit/types/Types.h"
#include "eckit/utils/MD5.h"
#include "eckit/value/Params.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Properties::Properties() {}

Properties::Properties(const property_t& value) {
    ASSERT(value.isOrderedMap() || value.isMap());
    ValueMap value_map = value;
    for (ValueMap::const_iterator vit = value_map.begin(); vit != value_map.end(); ++vit) {
        props_[vit->first] = vit->second;
    }
}

Properties::Properties(Stream& s) {
    s >> props_;
}

bool Properties::has(const key_t& k) const {
    return (props_.find(k) != props_.end());
}

Properties::property_t Properties::get(const key_t& k) const {
    PropertyMap::const_iterator vit = props_.find(k);
    if (vit != props_.end())
        return (*vit).second;
    else
        return property_t();  // return Nil Value...
}

Properties& Properties::set(const key_t& k, const property_t& v) {
    props_[k] = v;
    return *this;
}

Properties& Properties::set(const key_t& k, const Properties& p) {
    ValueMap pmap;
    for (PropertyMap::const_iterator vit = p.props_.begin(); vit != p.props_.end(); ++vit) {
        pmap[vit->first] = vit->second;
    }
    props_[k] = Value::makeMap(pmap);
    return *this;
}

Properties& Properties::set(const Properties& p) {
    for (PropertyMap::const_iterator vit = p.props_.begin(); vit != p.props_.end(); ++vit) {
        props_[vit->first] = vit->second;
    }
    return *this;
}

bool Properties::remove(const key_t& k) {
    return props_.erase(k);
}

void Properties::hash(MD5& md5) const {
    for (PropertyMap::const_iterator vit = props_.begin(); vit != props_.end(); ++vit) {
        md5.add((*vit).first);
        /// @note below, we assume all Values translate to std::string, this needs more verification
        md5.add((*vit).second.as<std::string>());
    }
}

void Properties::json(JSON& s) const {
    s << props_;
}

void Properties::print(std::ostream& s) const {
    for (PropertyMap::const_iterator vit = props_.begin(); vit != props_.end(); ++vit)
        s << "(" << (*vit).first << "," << (*vit).second << ")";
}

void Properties::encode(Stream& s) const {
    s << props_;
}

Properties::operator property_t() const {
    ValueMap vmap = Value::makeMap();
    for (PropertyMap::const_iterator vit = props_.begin(); vit != props_.end(); ++vit) {
        vmap[vit->first] = vit->second;
    }
    return vmap;
}

//----------------------------------------------------------------------------------------------------------------------

Properties::property_t getValue(const Properties& p, const Properties::key_t& key) {
    return p.get(key);
}

void print(const Properties& p, std::ostream& s) {
    s << p;
}

void encode(const Properties& p, Stream& s) {
    s << p;
}

Params::Factory<Properties> propertiesFactory;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
