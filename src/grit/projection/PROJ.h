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

#include "grit/Parametrisation.h"
#include "grit/Projection.h"
#include "grit/types.h"


namespace grit::projection {


/// Compute coordinates using PROJ (base class)
class PROJ : public Projection {
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

    // -- Exceptions
    // None

    // -- Constructors
    // None

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    const std::string& source() const { return source_; }
    const std::string& target() const { return target_; }
    std::string ellipsoid() const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    PROJ(const std::string& source, const std::string& target);

    // -- Members
    // None

    // -- Methods

    const pj_t& proj() const { return proj_; }

    // -- Overridden methods
    // None

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
    std::string ellipsoid_;

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


/// Compute coordinates using PROJ (specialization on input/output point types)
template <class PointSource, class PointTarget>
class PROJTT final : public PROJ {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit PROJTT(const Parametrisation& param) : PROJ(param.get_string("source"), param.get_string("target")) {}

    PROJTT(const std::string& source, const std::string& target) : PROJ(source, target) {}

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    PointTarget fwd(const PointSource&) const;
    PointSource inv(const PointTarget&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    Point fwd(const Point& p) const override { return {fwd(std::get<PointSource>(p))}; }
    Point inv(const Point& q) const override { return {inv(std::get<PointTarget>(q))}; }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


using PROJ_LatLon_to_LatLon = PROJTT<PointLatLon, PointLatLon>;
using PROJ_LatLon_to_XY     = PROJTT<PointLatLon, PointXY>;
using PROJ_LatLon_to_XYZ    = PROJTT<PointLatLon, PointXYZ>;
using PROJ_XY_to_LatLon     = PROJTT<PointXY, PointLatLon>;
using PROJ_XY_to_XY         = PROJTT<PointXY, PointXY>;
using PROJ_XY_to_XYZ        = PROJTT<PointXY, PointXYZ>;
using PROJ_XYZ_to_LatLon    = PROJTT<PointXYZ, PointLatLon>;
using PROJ_XYZ_to_XY        = PROJTT<PointXYZ, PointXY>;
using PROJ_XYZ_to_XYZ       = PROJTT<PointXYZ, PointXYZ>;


}  // namespace grit::projection
