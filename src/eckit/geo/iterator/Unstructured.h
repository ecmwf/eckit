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
#include <vector>

#include "eckit/geo/Iterator.h"


namespace eckit::geo::iterator {


class Unstructured final : public Iterator {
public:
    // -- Types

    struct Container {
        Container()          = default;
        virtual ~Container() = default;

        Container(const Container&) = delete;
        Container(Container&&)      = delete;

        Container& operator=(const Container&) = delete;
        Container& operator=(Container&&)      = delete;

        virtual Point get(size_t index) const = 0;
        virtual size_t size() const           = 0;
    };

    // -- Constructors

    explicit Unstructured(const Grid&, size_t index, const std::vector<double>& longitudes,
                          const std::vector<double>& latitudes);
    explicit Unstructured(const Grid&, size_t index, const std::vector<Point>&);
    explicit Unstructured(const Grid&, size_t index, std::vector<Point>&&);

    explicit Unstructured(const Grid&);

private:
    // -- Members

    std::unique_ptr<Container> container_;
    size_t index_;
    const size_t size_;
    const std::string uid_;

    // -- Overridden methods

    bool operator==(const geo::Iterator&) const override;
    bool operator++() override;
    bool operator+=(difference_type) override;

    explicit operator bool() const override;
    Point operator*() const override;

    size_t index() const override { return index_; }

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::iterator
