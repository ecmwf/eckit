/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/value/Params.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

bool Params::has(const std::string& key) const {
    return !getValue(*this, key).isNil();
}
/*
bool Params::get(const std::string& name, std::string& value) const
{
  if(!has(name)) return false;
  value = std::string(operator[](name));
  return true;
}
bool Params::get(const std::string& name, bool& value) const
{
  if(!has(name)) return false;
  value = operator[](name);
  return true;
}
bool Params::get(const std::string& name, long& value) const
{
  if(!has(name)) return false;
  value = operator[](name);
  return true;
}
bool Params::get(const std::string& name, size_t& value) const
{
  if(!has(name)) return false;
  value = operator[](name);
  return true;
}
bool Params::get(const std::string& name, double& value) const
{
  if(!has(name)) return false;
  value = operator[](name);
  return true;
}
bool Params::get(const std::string& name, std::vector<int>& value) const
{
  if(!has(name)) return false;
  std::vector<eckit::Value> v = operator[](name);
  value.assign(v.begin(),v.end());
  return true;
}
bool Params::get(const std::string& name, std::vector<long>& value) const
{
  if(!has(name)) return false;
  std::vector<eckit::Value> v = operator[](name);
  value.assign(v.begin(),v.end());
  return true;
}
bool Params::get(const std::string& name, std::vector<double>& value) const
{
  if(!has(name)) return false;
  std::vector<eckit::Value> v = operator[](name);
  value.assign(v.begin(),v.end());
  return true;
}
*/
Params::value_t Params::operator[](const Params::key_t& key) const {
    value_t v = getValue(*this, key);
    if (v.isNil())
        throw BadParameter("Params does not contain key: " + key, Here());
    return v;
}

Params::factory_map& Params::factories() {
    // Prevent static initialisation order fiasco by constructing the static
    // map on first use
    static factory_map factories_;
    return factories_;
}

Stream& operator<<(Stream& s, const Params& p) {
    encode(p, s);
    return s;
}

std::ostream& operator<<(std::ostream& s, const Params& p) {
    print(p, s);
    return s;
}

Params::value_t getValue(const Params& p, const Params::key_t& key) {
    return p.self_->get_(key);
}

void print(const Params& p, std::ostream& s) {
    p.self_->print_(s);
}

void encode(const Params& p, Stream& s) {
    p.self_->encode_(s);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
