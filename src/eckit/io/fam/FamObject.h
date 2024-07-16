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

namespace eckit {

class Buffer;
class FamSessionDetail;

//----------------------------------------------------------------------------------------------------------------------

class FamObject {
public:  // types

    using UPtr = std::unique_ptr<FamObject>;
    using SPtr = std::shared_ptr<FamObject>;

public:  // methods

    FamObject(FamSessionDetail& session, std::unique_ptr<FamObjectDescriptor> object);

    ~FamObject();

    // operators

    bool operator==(const FamObject& other) const;

    bool operator!=(const FamObject& other) const { return !operator==(other); }

    auto clone() const -> UPtr;

    void replaceWith(const FamDescriptor& object);

    void deallocate() const;

    auto exists() const -> bool;

    // properties

    auto regionId() const -> std::uint64_t;

    auto offset() const -> std::uint64_t;

    auto descriptor() const -> FamDescriptor { return {regionId(), offset()}; }

    auto size() const -> fam::size_t;

    auto permissions() const -> fam::perm_t;

    auto name() const -> std::string;

    auto property() const -> FamProperty;

    // data access

    void put(const void* buffer, fam::size_t offset, fam::size_t length) const;

    void get(void* buffer, fam::size_t offset, fam::size_t length) const;

    template <typename T>
    auto get(const fam::size_t offset) const -> T {
        auto buffer = T{0};
        get(&buffer, offset, sizeof(T));
        return buffer;
    }

    template <typename T>
    void put(const T& buffer, const fam::size_t offset) const {
        put(&buffer, offset, sizeof(T));
    }

    auto buffer(fam::size_t offset = 0) const -> Buffer;

    // atomic operations

    template <typename T>
    void set(fam::size_t offset, T value) const;

    template <typename T>
    auto fetch(fam::size_t offset) const -> T;

    template <typename T>
    void add(fam::size_t offset, T value) const;

    template <typename T>
    void subtract(fam::size_t offset, T value) const;

    template <typename T>
    auto swap(fam::size_t offset, T value) const -> T;

    template <typename T>
    auto compareSwap(fam::size_t offset, T oldValue, T newValue) const -> T;

private:  // methods

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamObject& object);

private:  // members

    std::shared_ptr<FamSessionDetail> session_;
    std::shared_ptr<FamObjectDescriptor> object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
