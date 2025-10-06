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

template <class T>
class SimpleOption : public BaseOption<T> {
public:

    using base_t = BaseOption<T>;
    using args_t = Option::args_t;

    SimpleOption(const std::string& name, const std::string& description);
    SimpleOption(const std::string& name, const std::string& description, const T& default_value);

    ~SimpleOption() override = default;

    size_t set(Configured&, size_t values, args_t::const_iterator begin, args_t::const_iterator end) const override;

protected:

    void print(std::ostream&) const override;

private:

    void set_value(const T& value, Configured&) const override;

    [[nodiscard]] T translate(const std::string& value) const override;

    void copy(const Configuration& from, Configured& to) const override;
};

}  // namespace eckit::option

#include "eckit/option/SimpleOption.cc"
