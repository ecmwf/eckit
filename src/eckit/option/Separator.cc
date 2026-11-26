// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Apr 2015


#include "eckit/option/Separator.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Translator.h"

#include <iostream>

namespace eckit::option {

Separator::Separator(const std::string& description) : Option("", description) {}

Separator::~Separator() {}

size_t Separator::set(Configured& parameter, size_t values, args_t::const_iterator begin,
                      args_t::const_iterator end) const {
    return 0;  // Never consumes any argv tokens
}

void Separator::setDefault(Configured&) const {
    ;
}

void Separator::copy(const Configuration& from, Configured& to) const {
    ;
}

bool Separator::active() const {
    return false;
}

void Separator::print(std::ostream& out) const {
    out << std::endl << description_ << ":" << std::endl;
}

}  // namespace eckit::option
