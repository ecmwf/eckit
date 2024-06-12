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

#include "detail/FamSessionDetail.h"
#include "eckit/exception/Exceptions.h"

#include <memory>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamObject::FamObject(FamSessionDetail& session, std::unique_ptr<FamObjectDescriptor> object):
    session_ {session.getShared()}, object_ {std::move(object)} {
    ASSERT(session_);
    ASSERT(object_);
}

FamObject::~FamObject() = default;

auto FamObject::clone() const -> UPtr {
    auto clone = std::make_unique<FamObjectDescriptor>(object_->get_global_descriptor());
    clone->set_used_memsrv_cnt(object_->get_used_memsrv_cnt());
    clone->set_memserver_ids(object_->get_memserver_ids());
    clone->set_size(object_->get_size());
    clone->set_perm(object_->get_perm());
    clone->set_name(object_->get_name());
    clone->set_desc_status(object_->get_desc_status());
    clone->set_interleave_size(object_->get_interleave_size());
    clone->set_uid(object_->get_uid());
    clone->set_gid(object_->get_gid());
    return std::make_unique<FamObject>(*session_, std::move(clone));
}

bool FamObject::operator==(const FamObject& other) const {
    const auto desc  = object_->get_global_descriptor();
    const auto oDesc = other.object_->get_global_descriptor();
    return (desc.regionId == oDesc.regionId && desc.offset == oDesc.offset);
}

//----------------------------------------------------------------------------------------------------------------------
// OPERATIONS

void FamObject::replaceWith(const FamDescriptor& object) {
    object_ = std::make_unique<FamObjectDescriptor>(Fam_Global_Descriptor {object.region, object.offset});
}

void FamObject::deallocate() const {
    session_->deallocateObject(*object_);
}

auto FamObject::exists() const -> bool {
    return (object_->get_desc_status() != Fam_Descriptor_Status::DESC_INVALID);
}

//----------------------------------------------------------------------------------------------------------------------
// PROPERTIES

auto FamObject::regionId() const -> std::uint64_t {
    return object_->get_global_descriptor().regionId;
}

auto FamObject::offset() const -> std::uint64_t {
    return object_->get_global_descriptor().offset;
}

auto FamObject::size() const -> fam::size_t {
    return object_->get_size();
}

auto FamObject::permissions() const -> fam::perm_t {
    return object_->get_perm();
}

auto FamObject::name() const -> std::string {
    return object_->get_name() ? object_->get_name() : "";
}

auto FamObject::property() const -> FamProperty {
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

//----------------------------------------------------------------------------------------------------------------------
// ATOMIC

template<typename T>
auto FamObject::fetch(const fam::size_t offset) const -> T {
    return session_->fetch<T>(*object_, offset);
}

template<typename T>
void FamObject::set(const fam::size_t offset, const T value) const {
    session_->set<T>(*object_, offset, value);
}

template<typename T>
void FamObject::add(const fam::size_t offset, const T value) const {
    session_->add<T>(*object_, offset, value);
}

template<typename T>
void FamObject::subtract(const fam::size_t offset, const T value) const {
    session_->subtract<T>(*object_, offset, value);
}

template<typename T>
auto FamObject::swap(const fam::size_t offset, const T value) const -> T {  // NOLINT
    return session_->swap<T>(*object_, offset, value);
}

template<typename T>
auto FamObject::compareSwap(const fam::size_t offset, const T oldValue, const T newValue) const -> T {
    return session_->compareSwap<T>(*object_, offset, oldValue, newValue);
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

template auto FamObject::fetch(const fam::size_t) const -> int32_t;
template auto FamObject::fetch(const fam::size_t) const -> int64_t;
template auto FamObject::fetch(const fam::size_t) const -> openfam::int128_t;
template auto FamObject::fetch(const fam::size_t) const -> uint32_t;
template auto FamObject::fetch(const fam::size_t) const -> uint64_t;
template auto FamObject::fetch(const fam::size_t) const -> float;
template auto FamObject::fetch(const fam::size_t) const -> double;

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

template auto FamObject::swap(const fam::size_t, const int32_t) const -> int32_t;
template auto FamObject::swap(const fam::size_t, const int64_t) const -> int64_t;
template auto FamObject::swap(const fam::size_t, const uint32_t) const -> uint32_t;
template auto FamObject::swap(const fam::size_t, const uint64_t) const -> uint64_t;
template auto FamObject::swap(const fam::size_t, const float) const -> float;
template auto FamObject::swap(const fam::size_t, const double) const -> double;

template auto FamObject::compareSwap(const fam::size_t, const int32_t, const int32_t) const -> int32_t;
template auto FamObject::compareSwap(const fam::size_t, const int64_t, const int64_t) const -> int64_t;
template auto FamObject::compareSwap(const fam::size_t, const uint32_t, const uint32_t) const -> uint32_t;
template auto FamObject::compareSwap(const fam::size_t, const uint64_t, const uint64_t) const -> uint64_t;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
