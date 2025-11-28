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

#include <cstddef>

#include "eckit/geo/Range.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


class Regular : public Range {
public:

    // -- Types

    struct Implementation {
        Implementation(Fraction inc, Fraction a, Fraction b, Fraction ref);

        Implementation(const Implementation&) = default;
        Implementation(Implementation&&)      = default;

        virtual ~Implementation() = default;

        Implementation& operator=(const Implementation&) = default;
        Implementation& operator=(Implementation&&)      = default;

        virtual Regular* make_cropped_range(double, double) const;
        virtual bool periodic() const;
        virtual bool includesNorthPole() const;
        virtual bool includesSouthPole() const;

        Fraction min() const;
        Fraction max() const;

        Fraction inc_;
        Fraction shift_;
        Fraction a_;
        Fraction b_;
        size_t size_ = 0;
    };

    // -- Overridden methods

    [[nodiscard]] const std::vector<double>& values() const override;

    [[nodiscard]] Regular* make_cropped_range(double crop_a, double crop_b) const override {
        return impl_->make_cropped_range(crop_a, crop_b);
    }

    size_t size() const override { return impl_->size_; }
    double a() const override { return impl_->a_; }
    double b() const override { return impl_->b_; }

    Fraction increment() const override { return impl_->inc_; }
    bool periodic() const override { return impl_->periodic(); }
    bool includesNorthPole() const override { return impl_->includesNorthPole(); }
    bool includesSouthPole() const override { return impl_->includesSouthPole(); }

    // -- Methods

    Fraction shift() const { return impl_->shift_; }
    Fraction af() const { return impl_->a_; }
    Fraction bf() const { return impl_->b_; }

protected:

    // -- Constructors

    explicit Regular(Implementation*);

private:

    // -- Members

    std::shared_ptr<Implementation> impl_;
};


struct RegularLatitude : Regular {
    explicit RegularLatitude(Fraction inc, Fraction a, Fraction b, Fraction ref = {});
    explicit RegularLatitude(double inc, double a, double b, double ref = 0) :
        RegularLatitude(Fraction{inc}, Fraction{a}, Fraction{b}, Fraction{ref}) {}

    [[nodiscard]] RegularLatitude* make_cropped_range(double crop_a, double crop_b) const override;
};


struct RegularLongitude : Regular {
    explicit RegularLongitude(Fraction inc, Fraction a, Fraction b, Fraction ref = {});
    explicit RegularLongitude(double inc, double a, double b, double ref = 0) :
        RegularLongitude(Fraction{inc}, Fraction{a}, Fraction{b}, Fraction{ref}) {}

    [[nodiscard]] RegularLongitude* make_cropped_range(double crop_a, double crop_b) const override;
};


struct RegularXY : Regular {
    explicit RegularXY(Fraction inc, Fraction a, Fraction b, Fraction ref = {});
    explicit RegularXY(double inc, double a, double b, double ref = 0) :
        RegularXY(Fraction{inc}, Fraction{a}, Fraction{b}, Fraction{ref}) {}

    [[nodiscard]] RegularXY* make_cropped_range(double crop_a, double crop_b) const override;
};


}  // namespace eckit::geo::range
