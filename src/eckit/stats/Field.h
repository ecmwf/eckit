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

#include <limits>
#include <memory>
#include <vector>


namespace eckit::stats {


class Field {
public:
    using values_type = std::vector<double>;
    using value_type  = values_type::value_type;

    struct Storage {
        virtual ~Storage() = default;

        virtual const values_type& values() const = 0;
        virtual value_type missingValue() const   = 0;
        virtual bool hasMissing() const;

        virtual void update(values_type&) = 0;
    };

private:
    Field(Storage*);
    std::unique_ptr<Storage> storage_;

public:
    explicit Field(size_t N, value_type missingValue = std::numeric_limits<value_type>::quiet_NaN());

    Field make_reference(values_type& values, value_type missingValue = std::numeric_limits<value_type>::quiet_NaN());
    Field make_instance(values_type&& values, value_type missingValue = std::numeric_limits<value_type>::quiet_NaN());

    const values_type& values() const { return storage_->values(); }
    value_type missingValue() const { return storage_->missingValue(); }
    bool hasMissing() const { return storage_->hasMissing(); }

    void update(values_type& values) { storage_->update(values); }
};


}  // namespace eckit::stats
