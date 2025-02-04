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


#include "eckit/stats/Field.h"

#include <algorithm>
#include <utility>

#include "eckit/exception/Exceptions.h"


namespace eckit::stats {


namespace {


using values_type = Field::values_type;
using value_type  = Field::value_type;


struct StoreReference : public Field::Storage {
    StoreReference(values_type& values, value_type missingValue) : values_(values), missingValue_(missingValue) {}

    const values_type& values() const override { return values_; }
    value_type missingValue() const override { return missingValue_; }

    void update(values_type& values) override { values_ = values; }

    values_type& values_;
    value_type missingValue_;
};


struct StoreInstance : public Field::Storage {
    StoreInstance(values_type&& values, value_type missingValue) : values_(values), missingValue_(missingValue) {}

    const values_type& values() const override { return values_; }
    value_type missingValue() const override { return missingValue_; }

    void update(values_type& values) override { values_ = values; }

    values_type values_;
    value_type missingValue_;
};


}  // namespace


bool Field::Storage::hasMissing() const {
    const auto miss = missingValue();
    return std::any_of(values().cbegin(), values().cend(), [miss](auto v) { return v != miss; });
}


Field::Field(Storage* storage) : storage_(storage) {
    ASSERT(storage_);
}


Field::Field(size_t N, value_type missingValue) :
    storage_{new StoreInstance(values_type(N, missingValue), missingValue)} {}


Field Field::make_reference(values_type& values, value_type missingValue) {
    return {new StoreReference(values, missingValue)};
}


Field Field::make_instance(values_type&& values, value_type missingValue) {
    return {new StoreInstance(std::move(values), missingValue)};
}


}  // namespace eckit::stats
