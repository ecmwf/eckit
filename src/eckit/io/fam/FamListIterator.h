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
    using reference  = Buffer&;

public:  // methods

    FamListIterator(const value_type& object);

    // iterate forwards
    auto operator++() -> FamListIterator&;

    // iterate backwards
    auto operator--() -> FamListIterator&;

    auto operator==(const FamListIterator& other) const -> bool;

    auto operator!=(const FamListIterator& other) const -> bool { return !operator==(other); }

    auto operator->() -> pointer;

    auto operator*() -> reference;

private:  // members

    bool invalid_{true};
    Buffer data_{0};

    value_type object_;
};

//----------------------------------------------------------------------------------------------------------------------
// CONST ITERATOR

class FamListConstIterator : public FamListIterator {
    using FamListIterator::FamListIterator;

    using value_type = FamListIterator::value_type;
    using pointer    = const value_type*;
    using reference  = const Buffer&;

public:  // methods

    auto operator->() -> pointer { return FamListIterator::operator->(); }

    auto operator*() -> reference { return FamListIterator::operator*(); }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
