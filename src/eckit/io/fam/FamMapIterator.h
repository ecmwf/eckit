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

/// @file   FamMapIterator.h
/// @author Metin Cakircali
/// @date   Jul 2024

#pragma once

#include "eckit/io/fam/FamList.h"
#include "eckit/io/fam/FamRegion.h"

#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
// ITERATOR

class FamMapIterator {
public:  // types
    using iterator_category = std::forward_iterator_tag;

    using value_type = FamList;
    using pointer    = value_type*;
    using reference  = value_type&;

public:  // methods
    FamMapIterator(const FamRegion& region, fam::index_t offset);

    auto operator++() -> FamMapIterator&;

    auto operator==(const FamMapIterator& other) const -> bool { return other.node_ == node_; }

    auto operator!=(const FamMapIterator& other) const -> bool { return !operator==(other); }

    auto operator->() -> pointer;

    auto operator*() -> reference;

private:  // members
    FamRegion region_;
    FamObject node_;

    std::unique_ptr<value_type> list_;
};

//----------------------------------------------------------------------------------------------------------------------
// CONST ITERATOR

class FamMapConstIterator: public FamMapIterator {
    using FamMapIterator::FamMapIterator;

    using value_type = FamMapIterator::value_type;
    using pointer    = const value_type*;
    using reference  = const value_type&;

public:  // methods
    auto operator->() -> pointer { return FamMapIterator::operator->(); }

    auto operator*() -> reference { return FamMapIterator::operator*(); }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
