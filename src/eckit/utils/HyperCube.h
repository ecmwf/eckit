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
/// @date   Oct 96

#ifndef eckit_utils_HyperCube_h
#define eckit_utils_HyperCube_h

#include <numeric>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/Types.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Helper class to handle multi-dimension objects
/// @note The first dimension should be the one most likely to change

class HyperCube {
public:  // types

    typedef std::vector<Ordinal> Dimensions;
    typedef std::vector<Ordinal> Coordinates;
    typedef std::vector<Ordinal> Remapping;

public:  // methods

    explicit HyperCube(const Dimensions& d) : dimensions_(d) {}

    /// Translate coordinates into an index to a 1 dimension array
    Ordinal index(const Coordinates&) const;

    /// @returns the number of elemets
    Ordinal count() const;

    /// Translate index to coordinates
    void coordinates(Ordinal index, Coordinates&) const;

    // Accessors

    const Dimensions& dimensions() const { return dimensions_; }
    Dimensions& dimensions() { return dimensions_; }
    Ordinal dimensions(Ordinal n) const { return dimensions_[n]; }
    Ordinal size() const { return dimensions_.size(); }

    /// @returns the 'remapping' std::vector needing to add 'count' labels for the dimension 'which' at position 'where'
    HyperCube addToDimension(Ordinal which, Ordinal where, Ordinal count, Remapping&) const;

    /// Combines two 'remapping' vectors
    static void combine(Remapping&, const Remapping&);

private:  // members

    Dimensions dimensions_;
};

//----------------------------------------------------------------------------------------------------------------------

/// Method is inlined for speed
inline Ordinal HyperCube::count() const {
    return std::accumulate(dimensions_.begin(), dimensions_.end(), 1, std::multiplies<Ordinal>());
}

/// Method is inlined for speed
inline Ordinal HyperCube::index(const Coordinates& coord) const {
    Ordinal n = 1;
    Ordinal a = 0;

    ASSERT(coord.size() == dimensions_.size());

    // The fact that this is in reverse is important for addToDimension

    for (int i = coord.size() - 1; i >= 0; i--) {
        ASSERT(/*coord[i] >= 0 &&*/ coord[i] < dimensions_[i]);
        a += coord[i] * n;
        n *= dimensions_[i];
    }

    return a;
}

/// Method is inlined for speed
inline void HyperCube::combine(Remapping& map1, const Remapping& map2) {
    if (map1.size() == 0) {
        map1 = map2;
    }
    else {
        for (Remapping::iterator i = map1.begin(); i != map1.end(); ++i) {
            ASSERT(*i < map2.size());
            *i = map2[*i];
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
