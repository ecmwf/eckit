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

#include "detail/FamObjectDetail.h"

#include <memory>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

FamObject::FamObject(std::unique_ptr<FamObjectDetail> object) noexcept: impl_ {std::move(object)} { }

FamObject::~FamObject() = default;

bool FamObject::operator==(const FamObject& other) const {
    const auto desc  = impl_->object()->get_global_descriptor();
    const auto oDesc = other.impl_->object()->get_global_descriptor();
    return (desc.regionId == oDesc.regionId && desc.offset == oDesc.offset);
}

void FamObject::replaceWith(const FamDescriptor& object) {
    impl_->replaceWith(object);
}

void FamObject::deallocate() {
    impl_->deallocate();
}

//----------------------------------------------------------------------------------------------------------------------
// DATA

void FamObject::put(const void* buffer, const fam::size_t offset, const fam::size_t length) const {
    impl_->put(buffer, offset, length);
}

void FamObject::get(void* buffer, const fam::size_t offset, const fam::size_t length) const {
    impl_->get(buffer, offset, length);
}

//----------------------------------------------------------------------------------------------------------------------
// ATOMIC

template<typename T>
auto FamObject::fetch(const fam::size_t offset) const -> T {
    return impl_->fetch<T>(offset);
}

template<typename T>
void FamObject::set(const fam::size_t offset, const T value) const {
    impl_->set<T>(offset, value);
}

template<typename T>
void FamObject::add(const fam::size_t offset, const T value) const {
    impl_->add<T>(offset, value);
}

template<typename T>
void FamObject::subtract(const fam::size_t offset, const T value) const {
    impl_->subtract<T>(offset, value);
}

template<typename T>
auto FamObject::swap(const fam::size_t offset, const T value) const -> T {  // NOLINT
    return impl_->swap<T>(offset, value);
}

template<typename T>
auto FamObject::compareSwap(const fam::size_t offset, const T oldValue, const T newValue) const -> T {
    return impl_->compareSwap<T>(offset, oldValue, newValue);
}

//----------------------------------------------------------------------------------------------------------------------

auto FamObject::regionId() const -> std::uint64_t {
    return impl_->descriptor().regionId;
}

auto FamObject::offset() const -> std::uint64_t {
    return impl_->descriptor().offset;
}

auto FamObject::size() const -> fam::size_t {
    return impl_->size();
}

auto FamObject::permissions() const -> fam::perm_t {
    return impl_->permissions();
}

auto FamObject::name() const -> std::string {
    return impl_->name();
}

//----------------------------------------------------------------------------------------------------------------------

void FamObject::print(std::ostream& out) const {
    out << "FamObject[" << *impl_ << "]";
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
