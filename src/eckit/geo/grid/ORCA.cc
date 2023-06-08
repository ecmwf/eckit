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


#include "eckit/geo/grid/ORCA.h"

#include <ostream>
#include <utility>
#include <vector>

#include "eckit/geo/Iterator.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::grid {


// order is important
static const std::vector<std::pair<std::string, std::string>> grib_keys{
    {"orca_name", "unstructuredGridType"}, {"orca_arrangement", "unstructuredGridSubtype"}, {"uid", "uuidOfHGrid"}};


ORCA::ORCA(const std::string& uid) :
    Grid(BoundingBox() /*assumed global*/), spec_(atlas::grid::SpecRegistry::get(uid)) {}


ORCA::ORCA(const Configuration& config) :
    ORCA([&config]() {
        std::string uid;
        ASSERT(config.get("uid", uid));
        return uid;
    }()) {}


ORCA::~ORCA() = default;


size_t ORCA::numberOfPoints() const {
    return static_cast<size_t>(atlasGridRef().size());
}


void ORCA::print(std::ostream& out) const {
    out << "ORCA[spec=" << spec_ << "]";
}


Iterator* ORCA::iterator() const {
    class ORCAIterator : public Iterator {
        ::atlas::Grid grid_;  // Note: needs the object because IterateLonLat uses a Grid reference
        ::atlas::Grid::IterateLonLat lonlat_;

        decltype(lonlat_)::iterator it_;
        decltype(lonlat_)::iterator::value_type p_;

        const size_t total_;
        size_t count_;
        bool first_;

        void print(std::ostream& out) const override {
            out << "ORCAIterator[";
            Iterator::print(out);
            out << ",count=" << count_ << ",total=" << total_ << "]";
        }

        bool next(double& _lat, double& _lon) override {
            if (it_.next(p_)) {
                point_[0] = p_.lat();
                point_[1] = p_.lon();
                _lat      = p_.lat();
                _lon      = p_.lon();

                if (first_) {
                    first_ = false;
                }
                else {
                    count_++;
                }

                return true;
            }

            ASSERT(count_ == total_);
            return false;
        }

        size_t index() const override { return count_; }

        size_t size() const override { NOTIMP; }

    public:
        ORCAIterator(::atlas::Grid grid) :
            grid_(grid),
            lonlat_(grid.lonlat()),
            it_(lonlat_.begin()),
            total_(size_t(grid.size())),
            count_(0),
            first_(true) {}

        ~ORCAIterator() override = default;

        ORCAIterator(const ORCAIterator&)            = delete;
        ORCAIterator(ORCAIterator&&)                 = delete;
        ORCAIterator& operator=(const ORCAIterator&) = delete;
        ORCAIterator& operator=(ORCAIterator&&)      = delete;
    };
    return new ORCAIterator(atlasGridRef());
}


const atlas::Grid& ORCA::atlasGridRef() const {
    return grid_ ? grid_ : (grid_ = atlas::Grid(spec_));
}


#if 0
Grid* GridBuilder<UnstructuredGrid>::make(const Configuration& config) {

    // specially-named unstructured grids
    std::string grid;
    if (config.get("grid", grid)) {
        if (!key::grid::ORCAPattern::match(grid, config).empty()) {
            return new other::ORCA(config);
        }
    }

    return new UnstructuredGrid(config);
}
#endif


}  // namespace eckit::geo::grid
