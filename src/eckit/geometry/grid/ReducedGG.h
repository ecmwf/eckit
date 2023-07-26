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


#pragma once

#include "eckit/geometry/grid/Gaussian.h"
#include "eckit/geometry/util.h"


namespace eckit {
class Fraction;
}


namespace eckit::geometry::grid {


class ReducedGG : public Gaussian {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ReducedGG(const Configuration&);

    ReducedGG(size_t N, const pl_type& pl, const area::BoundingBox& box = {}) :
        Gaussian(N, box), k_(0), Nj_(N_ * 2) {
        setNj(pl, box.south(), box.north());
    }

    // -- Destructor

    ~ReducedGG() override = default;

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    static pl_type pls(const std::string&);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    ReducedGG(size_t N, const area::BoundingBox& box = {}) :
        Gaussian(N, box), k_(0), Nj_(N * 2) {
        // derived classes must set k_, Nj_ using this constructor
    }

    // -- Members

    size_t k_;
    size_t Nj_;

    // -- Methods

    const pl_type& pls() const;

    void setNj(pl_type, double s, double n);
    void correctWestEast(double& w, double& e) const;

    // -- Overridden methods

    bool isPeriodicWestEast() const override;
    void print(std::ostream& out) const override;

    // -- Class members

    // -- Class methods
    // None

private:
    // -- Members

    pl_type pl_;

    // -- Methods
    // None

    // -- Overridden methods

    size_t size() const override;
    Fraction getSmallestIncrement() const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::grid
