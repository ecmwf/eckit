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

#include <memory>

#include "eckit/io/fam/FamList.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamRegion.h"

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

    FamMapIterator& operator++();

    bool operator==(const FamMapIterator& other) const { return other.node_ == node_; }

    bool operator!=(const FamMapIterator& other) const { return !operator==(other); }

    pointer operator->();

    reference operator*();

private:  // members

    FamRegion region_;
    FamObject node_;

    std::unique_ptr<value_type> list_;
};

//----------------------------------------------------------------------------------------------------------------------
// CONST ITERATOR

class FamMapConstIterator : public FamMapIterator {
    using FamMapIterator::FamMapIterator;

    using value_type = FamMapIterator::value_type;
    using pointer    = const value_type*;
    using reference  = const value_type&;

public:  // methods

    pointer operator->() { return FamMapIterator::operator->(); }

    reference operator*() { return FamMapIterator::operator*(); }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
