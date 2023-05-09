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
#include <variant>

#include "eckit/geo/Parametrisation.h"


namespace eckit::geo::param {


class Map final : public Parametrisation {
public:
    // -- Types

    using value_type =
        std::variant<bool, int, unsigned int, long, unsigned long, float, double, std::string, std::vector<bool>,
                     std::vector<int>, std::vector<unsigned int>, std::vector<long>, std::vector<unsigned long>,
                     std::vector<float>, std::vector<double>, std::vector<std::string>>;

    using container_type = std::map<key_type, value_type>;

    // -- Exceptions
    // None

    // -- Constructors

    explicit Map(const container_type& = {});

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void set(const key_type&, const bool&);
    void set(const key_type&, const int&);
    void set(const key_type&, const unsigned int&);
    void set(const key_type&, const long&);
    void set(const key_type&, const unsigned long&);
    void set(const key_type&, const float&);
    void set(const key_type&, const double&);
    void set(const key_type&, const std::string&);

    void set(const key_type&, const std::vector<bool>&);
    void set(const key_type&, const std::vector<int>&);
    void set(const key_type&, const std::vector<unsigned int>&);
    void set(const key_type&, const std::vector<long>&);
    void set(const key_type&, const std::vector<unsigned long>&);
    void set(const key_type&, const std::vector<float>&);
    void set(const key_type&, const std::vector<double>&);
    void set(const key_type&, const std::vector<std::string>&);

    // -- Overridden methods

    bool has(const key_type&) const override;

    bool get(const key_type&, bool&) const override;
    bool get(const key_type&, int&) const override;
    bool get(const key_type&, unsigned int&) const override;
    bool get(const key_type&, long&) const override;
    bool get(const key_type&, unsigned long&) const override;
    bool get(const key_type&, float&) const override;
    bool get(const key_type&, double&) const override;
    bool get(const key_type&, std::string&) const override;

    bool get(const key_type&, std::vector<bool>&) const override;
    bool get(const key_type&, std::vector<int>&) const override;
    bool get(const key_type&, std::vector<unsigned int>&) const override;
    bool get(const key_type&, std::vector<long>&) const override;
    bool get(const key_type&, std::vector<unsigned long>&) const override;
    bool get(const key_type&, std::vector<float>&) const override;
    bool get(const key_type&, std::vector<double>&) const override;
    bool get(const key_type&, std::vector<std::string>&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    container_type map_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::param
