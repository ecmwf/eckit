/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <iostream>

#include "griditer/util.h"


template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    const auto* sep = "";
    for (const auto& value : v) {
        out << sep << value;
        sep = ", ";
    }
    return out;
}


int main(int argc, char* argv[]) {

    std::cout << grit::util::linspace(1, 2, 2, true) << std::endl;
    std::cout << grit::util::arange(1, 2, 0.5) << std::endl;
}
