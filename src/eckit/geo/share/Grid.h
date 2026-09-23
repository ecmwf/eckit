// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <vector>


namespace eckit {
namespace spec {
class Spec;
}
class PathName;
}  // namespace eckit


namespace eckit::geo::share {


class Grid final {
public:

    static const Grid& instance();

private:

    // -- Constructors

    explicit Grid(const std::vector<PathName>&);

    // -- Members

    std::unique_ptr<spec::Spec> spec_;

    // -- Methods

    void load(const PathName&);
};


}  // namespace eckit::geo::share
