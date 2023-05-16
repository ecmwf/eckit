/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <memory>

#include "eccodes.h"

#include "eckit/config/Configuration.h"


namespace eckit::geo::grib {


// static eckit::Value v;

#if 0
class GribConfiguration final : std::unique_ptr<codes_handle, decltype(&codes_handle_delete)> {
private:
    // -- Types
    
    using t = std::unique_ptr<codes_handle, decltype(&codes_handle_delete)>;
    
    using cache_key_type = std::string;
    
    using cache_value_type = std::variant<bool, int, unsigned int, long, unsigned long, float, double, std::string, std::vector<bool>,
                                          std::vector<int>, std::vector<unsigned int>, std::vector<long>, std::vector<unsigned long>,
                                          std::vector<float>, std::vector<double>, std::vector<std::string>>;
    
    struct cache_type : protected std::map<cache_key_type, cache_value_type> {
        template <typename T>
        bool get(const key_type& key, T& value) const {
            if (auto it = find(key); it != end()) {
                value = std::get<T>(it->second);
                return true;
            }
            return false;
        }
        
        template <typename T>
        void set(const key_type& key, T& value) {
            operator[](key) = value;
        }
    };
    
public:
    // -- Exceptions
    // None
    
    // -- Constructors
    
    explicit GribConfiguration(codes_handle* h) :
        //        Configuration(v),
        t(h, &codes_handle_delete) { ASSERT(*this); }
    
    // -- Destructor
    // None
    
    // -- Convertors
    // None
    
    // -- Operators
    // None
    
    // -- Methods
    
    template <typename T>
    T get(const cache_key_type& key) const {
        auto value = T();
        ASSERT(get(key, value));
        return value;
    }
    
    // -- Overridden methods
    
    bool has(const cache_key_type& key) const /*override*/ { return 0 != codes_is_defined(t::get(), key.c_str()); }
    
    bool get(const cache_key_type& key, bool& value) const /*override*/ {
        if (long another = 0; get(key, another)) {
            value = another != 0;
            return true;
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, int& value) const /*override*/ {
        if (long another = 0; get(key, another)) {
            value = static_cast<int>(another);
            return true;
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, long& value) const /*override*/ {
        if (cache_.get(key, value)) {
            return true;
        }
        
        if (CODES_SUCCESS == codes_get_long(t::get(), key.c_str(), &value)) {
            cache_.set(key, value);
            return true;
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, unsigned long& value) const /*override*/ {
        if (long another = 0; get(key, another) && 0 <= another) {
            value = static_cast<unsigned long>(another);
            return true;
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, float& value) const /*override*/ {
        if (double another = 0; get(key, another)) {
            value = static_cast<float>(another);
            return true;
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, double& value) const /*override*/ {
        if (cache_.get(key, value)) {
            return true;
        }
        
        if (CODES_SUCCESS == codes_get_double(t::get(), key.c_str(), &value)) {
            cache_.set(key, value);
            return true;
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, std::string& value) const /*override*/ {
        if (cache_.get(key, value)) {
            return true;
        }
        
        char mesg[1024];
        if (auto length = sizeof(mesg); CODES_SUCCESS == codes_get_string(t::get(), key.c_str(), mesg, &length)) {
            value = mesg;
            cache_.set(key, value);
            return true;
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, std::vector<int>& value) const /*override*/ {
        if (std::vector<long> another; get(key, another)) {
            value.resize(another.size());
            std::transform(another.begin(), another.end(), value.begin(), [](long v) { return static_cast<int>(v); });
            return true;
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, std::vector<long>& value) const /*override*/ {
        if (cache_.get(key, value)) {
            return true;
        }
        
        if (size_t size = 0; CODES_SUCCESS == codes_get_size(t::get(), key.c_str(), &size)) {
            std::vector<long> another(size);
            if (CODES_SUCCESS == codes_get_long_array(t::get(), key.c_str(), another.data(), &size)) {
                value.swap(another);
                cache_.set(key, value);
                return true;
            }
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, std::vector<unsigned long>& value) const /*override*/ {
        if (std::vector<long> another; get(key, another)) {
            value.resize(another.size());
            std::transform(another.begin(), another.end(), value.begin(), [](long v) {
                ASSERT(0 <= v);
                return static_cast<unsigned long>(v);
            });
            return true;
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, std::vector<float>& value) const /*override*/ {
        if (cache_.get(key, value)) {
            return true;
        }
        
        if (size_t size = 0; CODES_SUCCESS == codes_get_size(t::get(), key.c_str(), &size)) {
            std::vector<float> another(size);
            if (CODES_SUCCESS == codes_get_float_array(t::get(), key.c_str(), another.data(), &size)) {
                value.swap(another);
                cache_.set(key, value);
                return true;
            }
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, std::vector<double>& value) const /*override*/ {
        if (cache_.get(key, value)) {
            return true;
        }
        
        if (size_t size = 0; CODES_SUCCESS == codes_get_size(t::get(), key.c_str(), &size)) {
            std::vector<double> another(size);
            if (CODES_SUCCESS == codes_get_double_array(t::get(), key.c_str(), another.data(), &size)) {
                value.swap(another);
                cache_.set(key, value);
                return true;
            }
        }
        
        return false;
    }
    
    bool get(const cache_key_type& key, std::vector<std::string>& value) const /*override*/ { NOTIMP; }
    
    
    // -- Class members
    // None
    
    // -- Class methods
    // None
    
private:
    // -- Members
    
    mutable cache_type cache_;
    
    // -- Methods
    // None
    
    // -- Overridden methods
    
    void print(std::ostream&) const /*override*/ {}
    
    // -- Class members
    // None
    
    // -- Class methods
    // None
    
    // -- Friends
    // None
};

}  // namespace eckit::geo::grib
