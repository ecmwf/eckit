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

#include <map>
#include <variant>

#include "eckit/geo/Spec.h"


namespace eckit {
class Value;
}


namespace eckit::geo::spec {


class Custom final : public Spec {
public:
    // -- Types

    struct key_type : std::string {
        key_type(const std::string&);
        key_type(const char* s) :
            key_type(std::string{s}) {};
    };

    using value_type = std::variant<std::string,
                                    bool,
                                    int,
                                    long,
                                    long long,
                                    size_t,
                                    float,
                                    double,
                                    std::vector<int>,
                                    std::vector<long>,
                                    std::vector<long long>,
                                    std::vector<size_t>,
                                    std::vector<float>,
                                    std::vector<double>,
                                    std::vector<std::string>>;

    using container_type = std::map<key_type, value_type>;

    // -- Constructors

    explicit Custom(const container_type& = {});
    explicit Custom(container_type&&);

    explicit Custom(const Value&);

    Custom(const Custom&);
    Custom(Custom&&);

    // -- Destructor

    ~Custom() override = default;

    // -- Operators

    Custom& operator=(const Custom&);
    Custom& operator=(Custom&&);

    bool operator==(const Custom&) const;

    // -- Methods

    bool empty() const { return map_.empty(); }
    void clear() { map_.clear(); }

    void json(JSON&) const override;

    void set(const std::string& name, const char* value) { set(name, std::string{value}); }

    void set(const std::string& name, const std::string& value);
    void set(const std::string& name, bool value);
    void set(const std::string& name, int value);
    void set(const std::string& name, long value);
    void set(const std::string& name, long long value);
    void set(const std::string& name, size_t value);
    void set(const std::string& name, float value);
    void set(const std::string& name, double value);
    void set(const std::string& name, const std::vector<int>& value);
    void set(const std::string& name, const std::vector<long>& value);
    void set(const std::string& name, const std::vector<long long>& value);
    void set(const std::string& name, const std::vector<size_t>& value);
    void set(const std::string& name, const std::vector<float>& value);
    void set(const std::string& name, const std::vector<double>& value);
    void set(const std::string& name, const std::vector<std::string>& value);

    // -- Overridden methods

    bool has(const std::string& name) const override;

    bool get(const std::string& name, std::string& value) const override;
    bool get(const std::string& name, bool& value) const override;
    bool get(const std::string& name, int& value) const override;
    bool get(const std::string& name, long& value) const override;
    bool get(const std::string& name, long long& value) const override;
    bool get(const std::string& name, size_t& value) const override;
    bool get(const std::string& name, float& value) const override;
    bool get(const std::string& name, double& value) const override;
    bool get(const std::string& name, std::vector<int>& value) const override;
    bool get(const std::string& name, std::vector<long>& value) const override;
    bool get(const std::string& name, std::vector<long long>& value) const override;
    bool get(const std::string& name, std::vector<size_t>& value) const override;
    bool get(const std::string& name, std::vector<float>& value) const override;
    bool get(const std::string& name, std::vector<double>& value) const override;
    bool get(const std::string& name, std::vector<std::string>& value) const override;

private:
    // -- Members

    container_type map_;
};


}  // namespace eckit::geo::spec
