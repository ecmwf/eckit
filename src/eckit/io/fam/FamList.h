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

#include "eckit/io/fam/FamListIterator.h"
#include "eckit/io/fam/FamRegion.h"

#include <memory>
#include <ostream>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamList {
public:  // types
    using iterator = FamListIterator;

    using const_iterator = FamListIterator;

public:  // methods
    FamList(FamRegion::SPtr region, const std::string& name);

    FamList(FamList&& other)            = default;
    FamList& operator=(FamList&& other) = default;

    ~FamList();

    auto size() const -> fam::size_t;

    auto empty() const -> bool;

    // iterators

    auto begin() -> iterator;

    auto end() -> iterator;

    auto cbegin() -> const_iterator;

    auto cend() -> const_iterator;

    // back

    auto back() -> Buffer { return *--end(); }

    void push_back(const void* data, fam::size_t length);

    void pop_back();

    // front

    auto front() -> Buffer { return *begin(); }

    void push_front(const void* data, fam::size_t length);

    void pop_front();

private:  // methods
    auto initSentinel(const std::string& name) -> FamObject::UPtr;

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamList& list);

private:  // members
    FamRegion::SPtr region_;
    FamObject::UPtr head_;
    FamObject::UPtr tail_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
