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

#include <memory>

#include <proj.h>

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Calculate coordinates using PROJ
class PROJ final : public Projection {
public:
    // -- Types

    struct pj_t : std::unique_ptr<PJ, decltype(&proj_destroy)> {
        using t = std::unique_ptr<PJ, decltype(&proj_destroy)>;
        explicit pj_t(PJ* ptr) : t(ptr, &proj_destroy) {}
        explicit operator PJ*() const { return t::get(); }
    };

    struct ctx_t : std::unique_ptr<PJ_CONTEXT, decltype(&proj_context_destroy)> {
        using t = std::unique_ptr<PJ_CONTEXT, decltype(&proj_context_destroy)>;
        explicit ctx_t(PJ_CONTEXT* ptr) : t(ptr, &proj_context_destroy) {}
        explicit operator PJ_CONTEXT*() const { return t::get(); }
    };

    struct Convert {
        Convert()          = default;
        virtual ~Convert() = default;

        Convert(const Convert&)        = delete;
        Convert(Convert&&)             = delete;
        void operator=(const Convert&) = delete;
        void operator=(Convert&&)      = delete;

        virtual PJ_COORD convert(const Point&) const = 0;
        virtual Point convert(const PJ_COORD&) const = 0;
    };

    // -- Exceptions
    // None

    // -- Constructors

    PROJ(const std::string& source, const std::string& target, double lon_minimum = 0.);
    explicit PROJ(const Parametrisation&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    const std::string& source() const { return source_; }
    const std::string& target() const { return target_; }

    static std::string ellipsoid(const std::string& string);

    // -- Overridden methods

    Point fwd(const Point&) const override;
    Point inv(const Point&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    pj_t proj_;
    ctx_t ctx_;

    const std::string source_;
    const std::string target_;

    std::unique_ptr<Convert> from_;
    std::unique_ptr<Convert> to_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::projection
