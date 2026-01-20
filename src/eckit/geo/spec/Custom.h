/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <initializer_list>
#include <map>
#include <memory>
#include <variant>

#include "eckit/geo/Spec.h"


namespace eckit {
class Value;
}


namespace eckit::geo::spec {


class Custom final : public Spec {
public:

    // -- Types

    struct custom_ptr : std::shared_ptr<Custom> {
        using shared_ptr::shared_ptr;
    };

    struct key_type : std::string {
        key_type(const std::string&);
        key_type(const char* s) : key_type(std::string{s}) {}
    };

    using value_type = std::variant<std::string, bool, int, long, long long, size_t, float, double, std::vector<int>,
                                    std::vector<long>, std::vector<long long>, std::vector<size_t>, std::vector<float>,
                                    std::vector<double>, std::vector<std::string>, custom_ptr,
                                    const char* /* converted to std::string */>;

    using container_type = std::map<key_type, value_type>;

    // -- Constructors

    Custom() = default;
    Custom(std::initializer_list<container_type::value_type>);

    explicit Custom(const container_type&);
    explicit Custom(container_type&&);

    // -- Operators

    bool operator==(const Custom&) const;
    bool operator!=(const Custom& other) const { return !operator==(other); }

    // -- Methods

    const container_type& container() const { return map_; }
    bool empty() const { return map_.empty(); }
    void clear() { map_.clear(); }

    void json(JSON&) const override;

    bool has_custom(const std::string& name) const;
    const custom_ptr& custom(const std::string& name) const;

    void set(const std::string& name, const std::string&);
    void set(const std::string& name, bool);
    void set(const std::string& name, int);
    void set(const std::string& name, long);
    void set(const std::string& name, long long);
    void set(const std::string& name, size_t);
    void set(const std::string& name, float);
    void set(const std::string& name, double);
    void set(const std::string& name, const std::vector<int>&);
    void set(const std::string& name, const std::vector<long>&);
    void set(const std::string& name, const std::vector<long long>&);
    void set(const std::string& name, const std::vector<size_t>&);
    void set(const std::string& name, const std::vector<float>&);
    void set(const std::string& name, const std::vector<double>&);
    void set(const std::string& name, const std::vector<std::string>&);

    void set(const std::string& name, const char* value) { set(name, std::string{value}); }
    void set(const std::string& name, const Value&);
    void set(const std::string& name, Custom*);

    // -- Overridden methods

    bool has(const std::string& name) const override;

    bool get(const std::string& name, std::string&) const override;
    bool get(const std::string& name, bool&) const override;
    bool get(const std::string& name, int&) const override;
    bool get(const std::string& name, long&) const override;
    bool get(const std::string& name, long long&) const override;
    bool get(const std::string& name, size_t&) const override;
    bool get(const std::string& name, float&) const override;
    bool get(const std::string& name, double&) const override;
    bool get(const std::string& name, std::vector<int>&) const override;
    bool get(const std::string& name, std::vector<long>&) const override;
    bool get(const std::string& name, std::vector<long long>&) const override;
    bool get(const std::string& name, std::vector<size_t>&) const override;
    bool get(const std::string& name, std::vector<float>&) const override;
    bool get(const std::string& name, std::vector<double>&) const override;
    bool get(const std::string& name, std::vector<std::string>&) const override;

    // -- Class methods

    [[nodiscard]] static Custom* make_from_value(const Value&);

private:

    // -- Members

    container_type map_;

    // -- Methods

    void set(const std::string& name, const custom_ptr&);
};


JSON& operator<<(JSON&, const Custom::custom_ptr&);


std::string to_string(const Custom::value_type&);


}  // namespace eckit::geo::spec
