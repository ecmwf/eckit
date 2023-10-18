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


#include "eckit/option/FactoryOption.h"

#include <iostream>

#include "eckit/config/Configuration.h"
#include "eckit/config/Configured.h"


namespace eckit::option {


template <class T>
void FactoryOption<T>::set(const std::string& value, Configured& parametrisation) const {
    parametrisation.set(name(), value);
}


template <class T>
void FactoryOption<T>::copy(const Configuration& from, Configured& to) const {
    if (std::string v; from.get(name(), v)) {
        to.set(name(), v);
    }
}


template <class T>
void FactoryOption<T>::print(std::ostream& out) const {
    T::list(out << "   --" << name() << "=name (" << description() << ")\n     Values are: ");
}


}  // namespace eckit::option
