/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <array>
#include <vector>

#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit {
class Configuration;
}  // namespace eckit


namespace eckit::geo {


using Triangle = std::array<size_t, 3>;


class ConvexHull {
public:
    // -- Types

    using builder_t = BuilderT1<ConvexHull>;
    using ARG1      = const Configuration&;

    // -- Constructors

    ConvexHull(const ConvexHull&) = delete;
    ConvexHull(ConvexHull&&)      = delete;

    // -- Operators

    void operator=(const ConvexHull&) = delete;
    void operator=(ConvexHull&&)      = delete;

    // -- Destructor

    virtual ~ConvexHull() = default;

    // -- Methods

    static std::string className() { return "ConvexHull"; }

protected:
    // -- Constructors

    ConvexHull() /*noexcept */ = default;
};


using ConvexHullFactory = Factory<ConvexHull>;

template <typename T>
using ConvexHullBuilder = ConcreteBuilderT1<ConvexHull, T>;


}  // namespace eckit::geo
