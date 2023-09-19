/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/option/MultiValueOption.h"

#include <algorithm>
#include <iostream>

#include "eckit/config/Configuration.h"
#include "eckit/config/Configured.h"

#include "eckit/exception/Exceptions.h"

namespace eckit::option {

MultiValueOption::MultiValueOption(const std::string& name, const std::string& description, size_t n_mandatory_values) :
    MultiValueOption(name, description, n_mandatory_values, 0) {}

MultiValueOption::MultiValueOption(const std::string& name, const std::string& description, size_t n_mandatory_values,
                                   size_t n_optional_values) :
    base_t(name, description),
    n_mandatory_values_{n_mandatory_values},
    n_optional_values_{n_optional_values},
    values_{} {
    ASSERT_MSG(n_mandatory_values >= 1, "At least 1 mandatory value is expected.");
}

MultiValueOption::MultiValueOption(const std::string& name, const std::string& description, size_t n_mandatory_values,
                                   const values_t& default_values) :
    MultiValueOption(name, description, n_mandatory_values, 0, default_values) {}

MultiValueOption::MultiValueOption(const std::string& name, const std::string& description, size_t n_mandatory_values,
                                   size_t n_maximum_values, const values_t& default_values) :
    base_t(name, description, default_values),
    n_mandatory_values_{n_mandatory_values},
    n_optional_values_{n_maximum_values},
    values_{} {
    ASSERT_MSG(n_mandatory_values >= 1, "At least 1 mandatory value is expected.");
}

size_t MultiValueOption::set(Configured& parametrisation, args_t::const_iterator begin,
                             args_t::const_iterator end) const {
    if (std::distance(begin, end) < n_mandatory_values_) {
        throw UserError("Not enough option values found for MultiValueOption, where at least "
                        + std::to_string(n_mandatory_values_) + " were expected");
    }

    // Collect n_mandatory_values_ mandatory values from the range [begin, end)
    values_t values;
    std::copy_n(begin, n_mandatory_values_, std::back_inserter(values));
    begin = begin + n_mandatory_values_;

    // Collect up to n_optional_values from the range [(updated-)begin, end)
    // - n.b. collection must stop when either end is reached or when an option (i.e. "--*") is found
    for (size_t i = 0; i < n_optional_values_; ++i) {
        if ((begin == end) || (begin->substr(0, 2) == "--")) {
            break;
        }

        values.push_back(*begin);
        ++begin;
    }

    // Store the collected values
    set_value(values, parametrisation);
    return values.size();
}

void MultiValueOption::set_value(const values_t& values, Configured& parametrisation) const {
    parametrisation.set(this->name(), values);
}

void MultiValueOption::copy(const Configuration& from, Configured& to) const {
    Option::copy<values_t>(this->name(), from, to);
}

void MultiValueOption::print(std::ostream& out) const {
    out << "   --" << this->name() << "[=] [" << n_mandatory_values_ << " * values]([" << n_optional_values_
        << " * values]) (" << this->description() << ")";
}

}  // namespace eckit::option
