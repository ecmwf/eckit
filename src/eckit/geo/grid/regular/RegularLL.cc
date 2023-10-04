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


#include "eckit/geo/grid/regular/RegularLL.h"

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geo/Increments.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/util/regex.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::regular {


RegularLL::RegularLL(const Configuration& config) :
    RegularLL({config.getDouble("west_east_increment"), config.getDouble("south_north_increment")},
              area::BoundingBox(config),
              {config.getDouble("west", 0.), config.getDouble("south", -90.)}) {}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox, const PointLonLat& reference) :
    Regular(bbox) {}


RegularLL::RegularLL(size_t ni, size_t nj, const area::BoundingBox& bbox, const PointLonLat& reference) :
    Regular(bbox), ni_(ni), nj_(nj) {}


RegularLL::RegularLL(const Increments& inc, const PointLonLat& reference) :
    RegularLL(inc, area::BoundingBox::make_global_prime(), reference) {}


RegularLL::RegularLL(size_t ni, size_t nj, const PointLonLat& reference) :
    RegularLL(ni, nj, area::BoundingBox::make_global_prime(), reference) {}


Grid::iterator RegularLL::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator RegularLL::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}

const std::vector<double>& RegularLL::longitudes() const {
    NOTIMP;
}


const std::vector<double>& RegularLL::latitudes() const {
    NOTIMP;
}


#define POSITIVE_REAL "[+]?([0-9]*[.])?[0-9]+([eE][-+][0-9]+)?"


Configuration* RegularLL::config(const std::string& name) {
    static const std::string pattern("(" POSITIVE_REAL ")/(" POSITIVE_REAL ")");

    auto match = util::regex_match(pattern, name);
    ASSERT(match);
    ASSERT(match.size() == 7);  // because of sub-matches

    auto d = Translator<std::string, double>{};
    std::vector<double> increments{d(match[1]), d(match[4])};

    auto ni = 1;  // detail::RegularIterator(Fraction(0), Fraction(360), Fraction(increments[0]), Fraction(0),
                  // Fraction(360)).n();
    auto nj = 1;  // detail::RegularIterator(Fraction(-90), Fraction(90), Fraction(increments[1]), Fraction(0)).n();

    return new MappedConfiguration({{"type", "regular_ll"},
                                    {"west_east_increment", increments[0]},
                                    {"south_north_increment", increments[1]},
                                    {"ni", ni},
                                    {"nj", nj}});
}


static const GridRegisterType<RegularLL> __grid_type("regular_ll");
static const GridRegisterName<RegularLL> __grid_pattern(POSITIVE_REAL "/" POSITIVE_REAL);


}  // namespace eckit::geo::grid::regular