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

#include "eckit/geo/ConvexHull.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit {
class Configuration;
}  // namespace eckit


namespace eckit::geo {


using Triangle = ConvexHull::Triangle;


class Triangulation : protected std::vector<Triangle> {
public:
    // -- Types

    using builder_t = BuilderT1<Triangulation>;
    using ARG1      = const Configuration&;

    // -- Constructors

    using vector::vector;

    Triangulation(const Triangulation&) = default;
    Triangulation(Triangulation&&)      = default;

    // -- Destructor

    virtual ~Triangulation() = default;

    // -- Operators

    using vector::operator[];

    Triangulation& operator=(const Triangulation&) = default;
    Triangulation& operator=(Triangulation&&)      = default;

    // -- Methods

    static std::string className() { return "Triangulation"; }

    using vector::at;
    using vector::size;

    using vector::begin;
    using vector::cbegin;
    using vector::cend;
    using vector::end;

protected:
    // -- Constructors

    explicit Triangulation(const std::vector<Triangle>& tri) :
        vector(tri) {}
};


using TriangulationFactory = Factory<Triangulation>;

template <typename T>
using TriangulationBuilder = ConcreteBuilderT1<Triangulation, T>;


}  // namespace eckit::geo
