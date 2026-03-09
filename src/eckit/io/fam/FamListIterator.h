/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamListIterator.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <iterator>
#include <optional>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// ITERATOR

class FamListIterator {
public:  // types

    using iterator_category = std::bidirectional_iterator_tag;

    using value_type = FamObject;
    using pointer    = value_type*;
    using reference  = value_type&;
    using data_type  = Buffer;

public:  // methods

    FamListIterator(value_type object);  // NOLINT(google-explicit-constructor)

    // iterate forwards
    FamListIterator& operator++();

    // iterate backwards
    FamListIterator& operator--();

    bool operator==(const FamListIterator& other) const;

    bool operator!=(const FamListIterator& other) const { return !operator==(other); }

    pointer operator->();

    data_type& operator*();

    value_type& object() { return object_; }

    const value_type& object() const { return object_; }

private:  // members

    value_type object_;

    std::optional<data_type> buffer_;
};

//----------------------------------------------------------------------------------------------------------------------
// CONST ITERATOR

class FamListConstIterator : public FamListIterator {
public:  // types

    using pointer   = const value_type*;
    using reference = const data_type&;

public:  // methods

    using FamListIterator::FamListIterator;

    pointer operator->() { return FamListIterator::operator->(); }

    reference operator*() { return FamListIterator::operator*(); }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
