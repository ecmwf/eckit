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

#include "eckit/io/fam/FamObject.h"

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

#include "fam/fam.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamProperty.h"
#include "eckit/io/fam/FamSession.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamObject::FamObject(FamSession& session, FamObjectDescriptor* object) :
    session_{session.shared_from_this()}, object_{object} {
    ASSERT(session_);
    ASSERT(object_);
}

FamObject::FamObject(FamSession& session, const std::uint64_t region, const std::uint64_t offset) :
    session_{session.shared_from_this()},
    object_{std::make_shared<FamObjectDescriptor>(Fam_Global_Descriptor{region, offset})} {
    ASSERT(session_);
    ASSERT(object_);
}

bool FamObject::operator==(const FamObject& other) const {
    const auto desc   = object_->get_global_descriptor();
    const auto o_desc = other.object_->get_global_descriptor();
    return (desc.regionId == o_desc.regionId && desc.offset == o_desc.offset);
}

//----------------------------------------------------------------------------------------------------------------------
// OPERATIONS

void FamObject::replaceWith(const FamDescriptor& object) {
    object_ = std::make_unique<FamObjectDescriptor>(Fam_Global_Descriptor{object.region, object.offset});
}

void FamObject::deallocate() const {
    session_->deallocateObject(*object_);
}

bool FamObject::exists() const {
    return (object_->get_desc_status() != openfam::Fam_Descriptor_Status::DESC_INVALID);
}

//----------------------------------------------------------------------------------------------------------------------
// PROPERTIES

fam::index_t FamObject::regionId() const {
    return object_->get_global_descriptor().regionId;
}

fam::index_t FamObject::offset() const {
    return object_->get_global_descriptor().offset;
}

fam::size_t FamObject::size() const {
    return object_->get_size();
}

fam::perm_t FamObject::permissions() const {
    return object_->get_perm();
}

std::string FamObject::name() const {
    return object_->get_name() ? object_->get_name() : "";
}

FamProperty FamObject::property() const {
    return {size(), permissions(), name(), object_->get_uid(), object_->get_gid()};
}

//----------------------------------------------------------------------------------------------------------------------
// DATA

void FamObject::put(const void* buffer, const fam::size_t offset, const fam::size_t length) const {
    session_->put(*object_, buffer, offset, length);
}

void FamObject::get(void* buffer, const fam::size_t offset, const fam::size_t length) const {
    session_->get(*object_, buffer, offset, length);
}

auto FamObject::data(const fam::size_t offset) const -> value_type {
    const auto size = this->size() - offset;
    Buffer buffer(size);
    get(buffer.data(), offset, size);
    return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
// ATOMIC

template <typename T>
T FamObject::fetch(const fam::size_t offset) const {
    return session_->fetch<T>(*object_, offset);
}

template <typename T>
void FamObject::set(const fam::size_t offset, const T value) const {
    session_->set<T>(*object_, offset, value);
}

template <typename T>
void FamObject::add(const fam::size_t offset, const T value) const {
    session_->add<T>(*object_, offset, value);
}

template <typename T>
void FamObject::subtract(const fam::size_t offset, const T value) const {
    session_->subtract<T>(*object_, offset, value);
}

template <typename T>
T FamObject::swap(const fam::size_t offset, const T value) const {  // NOLINT
    return session_->swap<T>(*object_, offset, value);
}

template <typename T>
T FamObject::compareSwap(const fam::size_t offset, const T old_value, const T new_value) const {
    return session_->compareSwap<T>(*object_, offset, old_value, new_value);
}

//----------------------------------------------------------------------------------------------------------------------

void FamObject::print(std::ostream& out) const {
    out << "FamObject[" << property() << ", region=" << regionId() << ", offset=" << offset() << "]";
}

std::ostream& operator<<(std::ostream& out, const FamObject& object) {
    object.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------
// forward instantiations

template int32_t FamObject::fetch(const fam::size_t) const;
template int64_t FamObject::fetch(const fam::size_t) const;
template openfam::int128_t FamObject::fetch(const fam::size_t) const;
template uint32_t FamObject::fetch(const fam::size_t) const;
template uint64_t FamObject::fetch(const fam::size_t) const;
template float FamObject::fetch(const fam::size_t) const;
template double FamObject::fetch(const fam::size_t) const;

template void FamObject::set(const fam::size_t, const int32_t) const;
template void FamObject::set(const fam::size_t, const int64_t) const;
template void FamObject::set(const fam::size_t, const openfam::int128_t) const;
template void FamObject::set(const fam::size_t, const uint32_t) const;
template void FamObject::set(const fam::size_t, const uint64_t) const;
template void FamObject::set(const fam::size_t, const float) const;
template void FamObject::set(const fam::size_t, const double) const;

template void FamObject::add(const fam::size_t, const int32_t) const;
template void FamObject::add(const fam::size_t, const int64_t) const;
template void FamObject::add(const fam::size_t, const uint32_t) const;
template void FamObject::add(const fam::size_t, const uint64_t) const;
template void FamObject::add(const fam::size_t, const float) const;
template void FamObject::add(const fam::size_t, const double) const;

template void FamObject::subtract(const fam::size_t, const int32_t) const;
template void FamObject::subtract(const fam::size_t, const int64_t) const;
template void FamObject::subtract(const fam::size_t, const uint32_t) const;
template void FamObject::subtract(const fam::size_t, const uint64_t) const;
template void FamObject::subtract(const fam::size_t, const float) const;
template void FamObject::subtract(const fam::size_t, const double) const;

template int32_t FamObject::swap(const fam::size_t, const int32_t) const;
template int64_t FamObject::swap(const fam::size_t, const int64_t) const;
template uint32_t FamObject::swap(const fam::size_t, const uint32_t) const;
template uint64_t FamObject::swap(const fam::size_t, const uint64_t) const;
template float FamObject::swap(const fam::size_t, const float) const;
template double FamObject::swap(const fam::size_t, const double) const;

template int32_t FamObject::compareSwap(const fam::size_t, const int32_t, const int32_t) const;
template int64_t FamObject::compareSwap(const fam::size_t, const int64_t, const int64_t) const;
template uint32_t FamObject::compareSwap(const fam::size_t, const uint32_t, const uint32_t) const;
template uint64_t FamObject::compareSwap(const fam::size_t, const uint64_t, const uint64_t) const;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
