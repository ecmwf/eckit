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

#include <algorithm>
#include <memory>
#include <unordered_set>

#include "eckit/geo/Spec.h"


namespace eckit::geo::spec {


class Layered final : public Spec {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Layered();
    explicit Layered(const Spec&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void hide(const std::string&);
    void unhide(const std::string&);
    void push_back(Spec*);
    void push_front(Spec*);

    // -- Overridden methods

    bool has(const std::string& name) const override {
        return !hide_.contains(name) &&
               (std::any_of(before_.begin(),
                            before_.end(),
                            [&](const decltype(before_)::value_type& c) { return c->has(name); }) ||
                spec_.has(name) ||
                std::any_of(after_.begin(), after_.end(), [&](const decltype(after_)::value_type& c) {
                    return c->has(name);
                }));
    }

    bool get(const std::string& name, std::string& value) const override { return __get(name, value); }
    bool get(const std::string& name, bool& value) const override { return __get(name, value); }
    bool get(const std::string& name, int& value) const override { return __get(name, value); }
    bool get(const std::string& name, long& value) const override { return __get(name, value); }
    bool get(const std::string& name, long long& value) const override { return __get(name, value); }
    bool get(const std::string& name, std::size_t& value) const override { return __get(name, value); }
    bool get(const std::string& name, float& value) const override { return __get(name, value); }
    bool get(const std::string& name, double& value) const override { return __get(name, value); }
    bool get(const std::string& name, std::vector<int>& value) const override { return __get(name, value); }
    bool get(const std::string& name, std::vector<long>& value) const override { return __get(name, value); }
    bool get(const std::string& name, std::vector<long long>& value) const override { return __get(name, value); }
    bool get(const std::string& name, std::vector<std::size_t>& value) const override { return __get(name, value); }
    bool get(const std::string& name, std::vector<float>& value) const override { return __get(name, value); }
    bool get(const std::string& name, std::vector<double>& value) const override { return __get(name, value); }
    bool get(const std::string& name, std::vector<std::string>& value) const override { return __get(name, value); }

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    struct : std::unordered_set<std::string> {
        bool contains(const value_type& name) const { return find(name) != end(); }
    } hide_;

    const Spec& spec_;
    std::vector<std::unique_ptr<Spec>> before_;
    std::vector<std::unique_ptr<Spec>> after_;

    // -- Methods

    template <typename T>
    bool __get(const std::string& name, T& value) const {
        return !hide_.contains(name) &&
               (std::any_of(before_.begin(),
                            before_.end(),
                            [&](const decltype(before_)::value_type& c) { return c->get(name, value); }) ||
                spec_.get(name, value) ||
                std::any_of(after_.begin(), after_.end(), [&](const decltype(after_)::value_type& c) {
                    return c->get(name, value);
                }));
    }

    // -- Overridden methods

    void print(std::ostream&) const final;
    void json(JSON&) const final;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::spec
