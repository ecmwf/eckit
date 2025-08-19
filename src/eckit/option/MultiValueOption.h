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

#include <iosfwd>

#include "eckit/option/Option.h"

namespace eckit::option {

class MultiValueOption : public BaseOption<std::vector<std::string>> {
public:

    using base_t   = BaseOption<std::vector<std::string>>;
    using values_t = std::vector<std::string>;

    MultiValueOption(const std::string& name, const std::string& description, size_t n_mandatory_values);
    MultiValueOption(const std::string& name, const std::string& description, size_t n_mandatory_values,
                     size_t n_optional_values);
    MultiValueOption(const std::string& name, const std::string& description, size_t n_mandatory_values,
                     const values_t& default_values);
    MultiValueOption(const std::string& name, const std::string& description, size_t n_mandatory_values,
                     size_t n_optional_values, const values_t& default_values);
    ~MultiValueOption() override = default;

    size_t set(Configured&, size_t values, args_t::const_iterator begin, args_t::const_iterator end) const override;

    void print(std::ostream&) const override;

private:

    MultiValueOption(const std::string& name, const std::string& description, size_t n_mandatory_values,
                     size_t n_maximum_values, std::optional<values_t> default_values);

    void set_value(const values_t& values, Configured&) const override;

    [[nodiscard]] values_t translate(const std::string& value) const override;

    void copy(const Configuration& from, Configured& to) const override;

    size_t n_mandatory_values_;
    size_t n_optional_values_;
    values_t values_;
};

}  // namespace eckit::option
