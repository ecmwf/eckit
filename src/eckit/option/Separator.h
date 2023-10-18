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
/// @author Pedro Maciel
/// @date Apr 2015


#pragma once

#include "eckit/option/Option.h"


namespace eckit::option {


class Separator final : public Option {
public:
    // -- Contructors

    explicit Separator(const std::string& description);

private:
    // -- Methods

    using Option::set;

    // -- Overridden methods

    void print(std::ostream&) const override;
    void set(const std::string& value, Configured&) const override;
    bool active() const override;
    void copy(const Configuration& from, Configured& to) const override;
};


}  // namespace eckit::option
