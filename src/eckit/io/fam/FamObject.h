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

/// @file   FamObject.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include "eckit/io/fam/FamProperty.h"

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace eckit {

class FamObjectDetail;

//----------------------------------------------------------------------------------------------------------------------

class FamObject {
public:  // types
    using UPtr = std::unique_ptr<FamObject>;

public:  // methods
    explicit FamObject(std::unique_ptr<FamObjectDetail> object) noexcept;

    ~FamObject();

    bool operator==(const FamObject& other) const;

    bool operator!=(const FamObject& other) const { return !operator==(other); }

    void replaceWith(const FamDescriptor& object);

    void deallocate();

    auto regionId() const -> std::uint64_t;

    auto offset() const -> std::uint64_t;

    auto descriptor() const -> FamDescriptor { return {regionId(), offset()}; }

    auto size() const -> fam::size_t;

    auto permissions() const -> fam::perm_t;

    auto name() const -> std::string;

    auto property() const -> FamProperty { return {size(), permissions(), name()}; }

    void put(const void* buffer, fam::size_t offset, fam::size_t length) const;

    void get(void* buffer, fam::size_t offset, fam::size_t length) const;

    template<typename T>
    auto get(const fam::size_t offset) const -> T {
        auto buffer = T {0};
        get(&buffer, offset, sizeof(T));
        return buffer;
    }

    template<typename T>
    void put(const T& buffer, const fam::size_t offset) const {
        put(&buffer, offset, sizeof(T));
    }

    template<typename T>
    void set(fam::size_t offset, T value) const;

    template<typename T>
    auto fetch(fam::size_t offset) const -> T;

    template<typename T>
    void add(fam::size_t offset, T value) const;

    template<typename T>
    auto swap(fam::size_t offset, T value) const -> T;

    template<typename T>
    auto compareSwap(fam::size_t offset, T oldValue, T newValue) const -> T;

private:  // methods
    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamObject& object);

private:  // members
    std::unique_ptr<FamObjectDetail> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
