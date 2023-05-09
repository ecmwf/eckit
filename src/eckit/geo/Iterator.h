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

#include "eckit/geo/types.h"


namespace eckit::geo {


class Iterator {
public:
    // -- Types

    using value_type = Point;

protected:
    // -- Types

    template <typename X>
    struct iterator_t {
        X& cnt;
        size_t pos;

        bool operator!=(const iterator_t& other) const { return &cnt != &other.cnt || pos != other.pos; }

        iterator_t& operator++() {
            pos++;
            return *this;
        }

        typename X::value_type& operator*() { return cnt.at(pos); }

        const typename X::value_type& operator*() const { return cnt.at(pos); }
    };

    // -- Constructors

    Iterator() = default;

public:
    // -- Types

    using iterator       = iterator_t<Iterator>;
    using const_iterator = iterator_t<const Iterator>;

    // -- Exceptions
    // None

    // -- Constructors

    Iterator(const Iterator&) = delete;
    Iterator(Iterator&&)      = delete;

    // -- Destructor

    virtual ~Iterator() = default;

    // -- Convertors
    // None

    // -- Operators

    Iterator& operator=(const Iterator&) = delete;
    Iterator& operator=(Iterator&&)      = delete;

    virtual bool operator++()    = 0;

    // -- Methods

    virtual size_t size() const = 0;

    iterator begin() { return {*this, 0}; }
    iterator end() { return {*this, this->size()}; }

    const_iterator cbegin() const { return {*this, 0}; }
    const_iterator cend() const { return {*this, this->size()}; }

    const_iterator begin() const { return cbegin(); }
    const_iterator end() const { return cend(); }

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
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace grit
