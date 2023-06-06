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

#include <iosfwd>
#include <memory>
#include <string>

#include "eckit/geo/Point.h"


namespace eckit {
class Configuration;
namespace geo {
class Projection;
}
}  // namespace eckit


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

    Iterator(Projection* = nullptr);

    Iterator(const Iterator&) = delete;
    Iterator(Iterator&&)      = delete;

    // -- Destructor

    virtual ~Iterator() = default;

    // -- Convertors

    operator bool() { return valid_; }

    // -- Operators

    Iterator& operator=(const Iterator&) = delete;
    Iterator& operator=(Iterator&&)      = delete;

    virtual bool operator++() = 0;

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

protected:
    // -- Members

    Point2 point_;
    std::unique_ptr<Projection> projection_;
    bool valid_;

    // -- Methods

    virtual void print(std::ostream&) const = 0;
    virtual size_t index() const            = 0;

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

    friend std::ostream& operator<<(std::ostream& s, const Iterator& p) {
        p.print(s);
        return s;
    }
};


struct IteratorFactory {
    using key_type = std::string;

    static Iterator* build(const key_type&, const Configuration&);
    static std::ostream& list(std::ostream&);

    IteratorFactory(const IteratorFactory&)            = delete;
    IteratorFactory(IteratorFactory&&)                 = delete;
    IteratorFactory& operator=(const IteratorFactory&) = delete;
    IteratorFactory& operator=(IteratorFactory&&)      = delete;

    virtual Iterator* make(const Configuration&) = 0;

protected:
    explicit IteratorFactory(const key_type&);
    virtual ~IteratorFactory();

private:
    const key_type key_;
};


template <class T>
class IteratorBuilder final : public IteratorFactory {
    Iterator* make(const Configuration& config) override { return new T(config); }

public:
    explicit IteratorBuilder(const IteratorFactory::key_type& key) :
        IteratorFactory(key) {}
};


}  // namespace eckit::geo
