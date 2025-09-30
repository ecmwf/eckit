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

/// @file   FamList.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <iosfwd>
#include <string>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamListIterator.h"
#include "eckit/io/fam/FamObject.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamRegion.h"
#include "eckit/io/fam/FamRegionName.h"

namespace eckit {

class FamRegionName;

//----------------------------------------------------------------------------------------------------------------------

class FamList {
public:  // types

    using size_type      = fam::size_t;
    using iterator       = FamListIterator;
    using const_iterator = FamListConstIterator;

    struct Descriptor {
        fam::index_t region{0};  // region ID
        fam::index_t head{0};    // offset of head sentinel
        fam::index_t tail{0};    // offset of tail sentinel
        fam::index_t size{0};    // offset of size sentinel
    };

public:  // methods


    FamList(const FamRegion& region, const Descriptor& desc);

    FamList(const FamRegion& region, const std::string& list_name);

    explicit FamList(const FamRegionName& name);

    // rules
    // FamList(const FamList&)            = default;
    // FamList& operator=(const FamList&) = default;
    // FamList(FamList&&)                 = delete;
    // FamList& operator=(FamList&&)      = delete;
    // ~FamList() = default;

    auto descriptor() const -> Descriptor;

    // capacity

    auto size() const -> size_type;

    auto empty() const -> bool;

    // iterators

    auto begin() const -> iterator;

    auto cbegin() const -> const_iterator;

    auto end() const -> iterator;

    auto cend() const -> const_iterator;

    // accessors

    auto front() const -> Buffer;

    auto back() const -> Buffer;

    // modifiers

    // void clear() noexcept;

    void pushBack(const void* data, size_type length);

    void pushFront(const void* data, size_type length);

    void popFront();

    void popBack();

private:  // methods

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamList& list);

private:  // members

    FamRegion region_;
    FamObject head_;
    FamObject tail_;
    FamObject size_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
