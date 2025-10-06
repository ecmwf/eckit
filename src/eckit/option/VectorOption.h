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
class VectorOption : public BaseOption<std::vector<T>> {
public:

    using base_t = BaseOption<std::vector<T>>;
    using args_t = Option::args_t;
    // -- Exceptions
    // None

    // -- Contructors

    VectorOption(const std::string& name, const std::string& description, size_t size, const char* separator = "/");
    VectorOption(const std::string& name, const std::string& description, size_t size,
                 const std::vector<T>& default_value, const char* separator = "/");

    // -- Destructor

    ~VectorOption() override = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    size_t set(Configured&, size_t values, args_t::const_iterator begin, args_t::const_iterator end) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members

    // -- Methods

    void print(std::ostream&) const override;  // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    VectorOption(const VectorOption&);
    VectorOption& operator=(const VectorOption&);

    // -- Members

    size_t size_;
    const char* separator_;

    // -- Methods
    // None

    // -- Overridden methods
    void set_value(const std::vector<T>& value, Configured&) const override;

    [[nodiscard]] std::vector<T> translate(const std::string& value) const override;

    void copy(const Configuration& from, Configured& to) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
};

}  // namespace eckit::option

#include "eckit/option/VectorOption.cc"
