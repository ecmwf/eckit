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
#include <vector>

#include "eckit/geometry/util.h"


template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    const auto* sep = "";
    for (const auto& value : v) {
        out << sep << value;
        sep = ", ";
    }
    return out;
}


template <typename X>
struct iterator_t {
    X& cnt;
    size_t pos;

    bool operator!=(const iterator_t& other) const { return &cnt != &other.cnt || pos != other.pos; }

    iterator_t& operator++() {
        pos++;
        return *this;
    }

    iterator_t operator++(int) {
        auto old = *this;
        operator++();
        return old;
    }

    typename X::value_type& operator*() { return cnt.at(pos); }

    const typename X::value_type& operator*() const { return cnt.at(pos); }
};


class iterable_t : public std::vector<double> {
    using iterator       = iterator_t<iterable_t>;
    using const_iterator = iterator_t<const iterable_t>;

public:
    using vector::vector;

    iterator begin() { return {*this, 0}; }
    iterator end() { return {*this, this->size()}; }
    const_iterator cbegin() const { return {*this, 0}; }
    const_iterator cend() const { return {*this, this->size()}; }
    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }
};


int main(int argc, char* argv[]) {
    using namespace eckit::geometry::util;

#if 0
    std::cout << linspace(1, 2, 2, true) << std::endl;
    std::cout << arange(1, 2, 0.5) << std::endl;
    std::cout << gaussian_latitudes(64, false) << std::endl;
    std::cout << normalise_longitude_to_maximum(0., 360.) << std::endl;
    std::cout << normalise_longitude_to_minimum(0., -360.) << std::endl;
#elif 1
    std::cout << reduced_classical_pl(16) << std::endl;
    std::cout << reduced_classical_pl(16) << std::endl;
    std::cout << reduced_octahedral_pl(16) << std::endl;
    std::cout << reduced_octahedral_pl(16) << std::endl;
#elif 0
    std::vector<double> values1{1., 2., 3., 4., 5., 6.};
    std::vector<double> values2{6., 5., 4., 3., 2., 1.};

    std::cout << monotonic_crop({1.}, 1., 1., 0.) << std::endl;
    std::cout << monotonic_crop({1., 1., 1.}, 1., 2., 0.) << std::endl;
    std::cout << monotonic_crop(values1, 2., 3., 0.) << std::endl;
    std::cout << monotonic_crop(values2, 2., 3., 0.) << std::endl;
#endif
}
