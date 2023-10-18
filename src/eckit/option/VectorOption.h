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


template <class T>
class VectorOption final : public Option {
public:
    // -- Contructors

    VectorOption(const std::string& name, const std::string& description, size_t size, const char* separator = "/",
                 bool keepEmpty = false);

private:
    // -- Members

    size_t size_;
    const char* separator_;
    bool keepEmpty_;

    // -- Overridden methods

    void print(std::ostream&) const override;
    void set(Configured&) const override;
    void set(const std::string& value, Configured&) const override;
    void copy(const Configuration& from, Configured& to) const override;
};


}  // namespace eckit::option


#include "eckit/option/VectorOption.cc"
