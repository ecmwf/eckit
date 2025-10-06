/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Apr 2015

#pragma once

#include <iosfwd>

#include "eckit/option/Option.h"

namespace eckit::option {

/// @note Factory option essentially provides a std::string option, whose acceptable values are listed on the
///       command line (typename T may be any class that implements the list() method). It does no checks on
///       the validity of input received, and just returns the appropriate string

template <class T>
class FactoryOption : public BaseOption<std::string> {
public:

    using base_t = BaseOption<std::string>;

    FactoryOption(const std::string& name, const std::string& description);
    FactoryOption(const std::string& name, const std::string& description, std::string default_value);

    ~FactoryOption() override = default;

    size_t set(Configured&, size_t values, args_t::const_iterator begin, args_t::const_iterator end) const override;

protected:

    void print(std::ostream&) const override;

private:

    void set_value(const std::string& value, Configured&) const override;

    [[nodiscard]] std::string translate(const std::string& value) const override;

    void copy(const Configuration& from, Configured& to) const override;
};

}  // namespace eckit::option

#include "eckit/option/FactoryOption.cc"
