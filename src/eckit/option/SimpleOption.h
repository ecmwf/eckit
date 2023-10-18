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

#include <iostream>

#include "eckit/config/Configuration.h"
#include "eckit/config/Configured.h"
#include "eckit/option/Option.h"
#include "eckit/utils/Translator.h"


namespace eckit {
class PathName;
}


namespace eckit::option {


template <class T>
class SimpleOption final : public Option {
public:
    // -- Contructors

    using Option::Option;

private:
    // -- Overridden methods

    void print(std::ostream& out) const override {
        out << "   --" << name() << "=" << Title<T>()() << " (" << description() << ")";
    }

    void set(Configured& parametrisation) const override { Option::set(parametrisation); }

    void set(const std::string& value, Configured& parametrisation) const override {
        T v = eckit::Translator<std::string, T>()(value);
        parametrisation.set(name(), v);
    }

    void copy(const Configuration& from, Configured& to) const override {
        if (T v; from.get(name(), v)) {
            to.set(name(), v);
        }
    }
};


template <>
void SimpleOption<eckit::PathName>::set(const std::string& value, Configured&) const;


template <>
void SimpleOption<eckit::PathName>::copy(const Configuration& from, Configured& to) const;


template <>
void SimpleOption<bool>::print(std::ostream&) const;


template <>
void SimpleOption<bool>::set(Configured&) const;


}  // namespace eckit::option
