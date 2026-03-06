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

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>

#include "eckit/io/fam/FamProperty.h"

namespace eckit {

class Buffer;
class FamSession;

//----------------------------------------------------------------------------------------------------------------------

class FamObject {
public:  // types

    using value_type = Buffer;

public:  // methods

    FamObject(FamSession& session, FamObjectDescriptor* object);

    FamObject(FamSession& session, std::uint64_t region, std::uint64_t offset);

    ~FamObject() = default;

    // rules: move-only
    FamObject(const FamObject&)            = delete;
    FamObject& operator=(const FamObject&) = delete;
    FamObject(FamObject&&)                 = default;
    FamObject& operator=(FamObject&&)      = default;

    // operators

    bool operator==(const FamObject& other) const;

    bool operator!=(const FamObject& other) const { return !operator==(other); }

    void replaceWith(const FamDescriptor& object);

    void deallocate() const;

    bool exists() const;

    // properties

    fam::index_t regionId() const;

    fam::index_t offset() const;

    FamDescriptor descriptor() const { return {regionId(), offset()}; }

    fam::size_t size() const;

    fam::perm_t permissions() const;

    std::string name() const;

    FamProperty property() const;

    // data access

    void put(const void* buffer, fam::size_t offset, fam::size_t length) const;

    void get(void* buffer, fam::size_t offset, fam::size_t length) const;

    template <typename T>
    T get(const fam::size_t offset = 0) const {
        auto buffer = T{0};
        get(&buffer, offset, sizeof(T));
        return buffer;
    }

    template <typename T>
    void put(const T& buffer, const fam::size_t offset) const {
        put(&buffer, offset, sizeof(T));
    }

    value_type data(fam::size_t offset = 0) const;

    // atomic operations

    template <typename T>
    void set(fam::size_t offset, T value) const;

    template <typename T>
    T fetch(fam::size_t offset) const;

    template <typename T>
    void add(fam::size_t offset, T value) const;

    template <typename T>
    void subtract(fam::size_t offset, T value) const;

    template <typename T>
    T swap(fam::size_t offset, T value) const;

    template <typename T>
    T compareSwap(fam::size_t offset, T old_value, T new_value) const;

private:  // methods

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const FamObject& object);

private:  // members

    std::shared_ptr<FamSession> session_;
    std::shared_ptr<FamObjectDescriptor> object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
