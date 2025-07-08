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


#include "eckit/geo/iterator/Unstructured.h"

#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/container/PointsContainer.h"
#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::iterator {


struct Instance {
    explicit Instance(const Spec& spec) : grid(dynamic_cast<const grid::Unstructured*>(GridFactory::build(spec))) {
        ASSERT(grid);
    }

    std::unique_ptr<const grid::Unstructured> grid;
};


struct UnstructuredInstance : Instance, Unstructured {
    explicit UnstructuredInstance(const Spec& spec) : Instance(spec), Unstructured(*grid) {}
};


Unstructured::Unstructured(const Grid& grid, size_t index, std::shared_ptr<container::PointsContainer> container) :
    container_(container), index_(index), size_(container_->size()), uid_(grid.uid()) {
    ASSERT(container_->size() == grid.size());
}


Unstructured::Unstructured(const Grid& grid) : index_(grid.size()), size_(grid.size()), uid_(grid.uid()) {}


bool Unstructured::operator==(const geo::Iterator& other) const {
    const auto* another = dynamic_cast<const Unstructured*>(&other);
    return another != nullptr && index_ == another->index_ && uid_ == another->uid_;
}


bool Unstructured::operator++() {
    if (index_++; index_ < size_) {
        return true;
    }

    index_ = size_;  // ensure it's invalid
    return false;
}


bool Unstructured::operator+=(difference_type d) {
    if (auto di = static_cast<difference_type>(index_); 0 <= di + d && di + d < static_cast<difference_type>(size_)) {
        index_ = static_cast<size_t>(di + d);
        return true;
    }

    index_ = size_;  // ensure it's invalid
    return false;
}


Unstructured::operator bool() const {
    return index_ < size_;
}


Point Unstructured::operator*() const {
    ASSERT(container_);
    return container_->get(index_);
}


void Unstructured::fill_spec(spec::Custom&) const {
    // FIXME implement
}


static const IteratorRegisterType<UnstructuredInstance> ITERATOR_TYPE("unstructured");


}  // namespace eckit::geo::iterator
