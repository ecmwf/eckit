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


#include <memory>

#include "grit/types.h"


namespace grit {
struct Figure;
struct Scanner;
struct Transformation;
}  // namespace grit


namespace grit {


class Iterator final {
public:
    // -- Types

    using value_type = Point;

private:
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

        iterator_t operator++(int) {
            auto old = *this;
            operator++();
            return old;
        }

        typename X::value_type& operator*() { return cnt.at(pos); }

        const typename X::value_type& operator*() const { return cnt.at(pos); }
    };

public:
    // -- Types

    using iterator       = iterator_t<Iterator>;
    using const_iterator = iterator_t<const Iterator>;

    // -- Exceptions
    // None

    // -- Constructors

    Iterator(Scanner* scanner, Figure* figure, Transformation* transformation);

    Iterator(const Iterator&) = delete;
    Iterator(Iterator&&)      = delete;

    Iterator& operator=(const Iterator&) = delete;
    Iterator& operator=(Iterator&&)      = delete;

    // -- Destructor

    ~Iterator() = delete;

    // -- Convertors
    // None

    // -- Operators

    bool operator++();
    bool operator++(int) { return operator++(); }

    // -- Methods

    size_t size() const;

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

    std::unique_ptr<Scanner> scanner_;
    std::unique_ptr<Figure> figure_;
    std::unique_ptr<Transformation> transformation_;

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
