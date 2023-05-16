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

#include <map>
#include <memory>

#include "eccodes.h"

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"


namespace eckit::geo::grib {


class GribConfiguration final : Configuration {
private:
    // -- Types

    using codes_handle_type = std::unique_ptr<codes_handle, decltype(&codes_handle_delete)>;

    using cache_value_type = std::variant<bool,
                                          int,
                                          unsigned int,
                                          long,
                                          unsigned long,
                                          float,
                                          double,
                                          std::string,
                                          std::vector<bool>,
                                          std::vector<int>,
                                          std::vector<unsigned int>,
                                          std::vector<long>,
                                          std::vector<unsigned long>,
                                          std::vector<float>,
                                          std::vector<double>,
                                          std::vector<std::string>>;

    struct cache_type : protected std::map<std::string, cache_value_type> {
        template <typename T>
        bool get(const key_type& name, T& value) const {
            if (auto it = find(name); it != end()) {
                value = std::get<T>(it->second);
                return true;
            }

            return false;
        }

        template <typename T>
        void set(const key_type& name, T& value) {
            operator[](name) = value;
        }
    };

public:
    // -- Exceptions
    // None

    // -- Constructors

    explicit GribConfiguration(codes_handle* h);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    template <typename T>
    T get(const std::string& name) const {
        auto value = T();
        ASSERT(get(name, value));
        return value;
    }

    // -- Overridden methods

    bool has(const std::string& name) const override;

    bool get(const std::string& name, std::string& value) const override;
    bool get(const std::string& name, bool& value) const override;
    bool get(const std::string& name, int& value) const override;
    bool get(const std::string& name, long& value) const override;
    bool get(const std::string& name, long long& value) const override;
    bool get(const std::string& name, std::size_t& value) const override;
    bool get(const std::string& name, float& value) const override;
    bool get(const std::string& name, double& value) const override;

    bool get(const std::string& name, std::vector<int>& value) const override;
    bool get(const std::string& name, std::vector<long>& value) const override;
    bool get(const std::string& name, std::vector<long long>& value) const override;
    bool get(const std::string& name, std::vector<std::size_t>& value) const override;
    bool get(const std::string& name, std::vector<float>& value) const override;
    bool get(const std::string& name, std::vector<double>& value) const override;
    bool get(const std::string& name, std::vector<std::string>& value) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    codes_handle_type codes_handle_;
    mutable cache_type cache_;

    // -- Methods
    // None

    // -- Overridden methods

    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};

}  // namespace eckit::geo::grib
