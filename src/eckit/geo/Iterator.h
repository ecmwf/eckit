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

#include <cstddef>
#include <iterator>

#include "eckit/geo/Point.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit::geo {
class Grid;
}  // namespace eckit::geo


namespace eckit::geo {


class Iterator {
public:

    // -- Types

    using builder_t = BuilderT1<Iterator>;
    using ARG1      = const Spec&;

    using iterator_category = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = Point;
    using pointer           = value_type*;
    using reference         = value_type&;

    // -- Constructors

    Iterator(const Iterator&) = default;
    Iterator(Iterator&&)      = default;

    // -- Destructor

    virtual ~Iterator() = default;

    // -- Operators

    Iterator& operator=(const Iterator&) = default;
    Iterator& operator=(Iterator&&)      = default;

    virtual bool operator==(const Iterator&) const = 0;
    bool operator!=(const Iterator& other) const { return !operator==(other); }

    virtual bool operator++()                = 0;
    virtual bool operator+=(difference_type) = 0;

    virtual bool operator--() { return operator-=(1); }
    virtual bool operator-=(difference_type diff) { return operator+=(-diff); }

    virtual explicit operator bool() const = 0;
    virtual value_type operator*() const   = 0;

    // -- Methods

    virtual size_t index() const = 0;

    // -- Class methods

    static std::string className() { return "iterator"; }

protected:

    // -- Constructors

    Iterator() = default;

private:

    // -- Friends

    friend class Grid;
};


using IteratorFactoryType = Factory<Iterator>;


template <typename T>
using IteratorRegisterType = ConcreteBuilderT1<Iterator, T>;


struct IteratorFactory {
    [[nodiscard]] static Iterator* build(const Spec& spec) { return instance().build_(spec); }
    [[nodiscard]] static Spec* make_spec(const Spec& spec) { return instance().make_spec_(spec); }

    static std::ostream& list(std::ostream& out) { return instance().list_(out); }

private:

    static IteratorFactory& instance();

    [[nodiscard]] Iterator* build_(const Spec&) const;
    [[nodiscard]] Spec* make_spec_(const Spec&) const;
    std::ostream& list_(std::ostream&) const;
};


}  // namespace eckit::geo
