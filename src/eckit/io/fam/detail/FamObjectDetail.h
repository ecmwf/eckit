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

/// @file   FamObjectDetail.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include "FamSessionDetail.h"
#include "eckit/exception/Exceptions.h"

#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamObjectDetail {
public:  // methods
    FamObjectDetail(FamSessionDetail& session, std::unique_ptr<FamObjectDescriptor> object):
        session_ {session.getShared()}, object_ {std::move(object)} {
        ASSERT(object_);
    }

    void replaceWith(const FamDescriptor& object) {
        object_ = std::make_unique<FamObjectDescriptor>(FamGlobalDescriptor {object.region, object.offset});
    }

    auto object() -> FamObjectDescriptor* { return object_.get(); }

    auto object() const -> const FamObjectDescriptor* { return object_.get(); }

    auto descriptor() const -> FamGlobalDescriptor { return object_->get_global_descriptor(); }

    auto name() const -> std::string { return object_->get_name() ? object_->get_name() : ""; }

    auto size() const -> fam::size_t { return object_->get_size(); }

    auto permissions() const -> fam::perm_t { return object_->get_perm(); }

    auto status() const -> int { return object_->get_desc_status(); }

    auto stat() const -> FamProperty { return session_->statObject(*object_); }

    void deallocate() { session_->deallocateObject(*object_); }

    void put(const void* buffer, const fam::size_t offset, const fam::size_t length) {
        session_->put(*object_, buffer, offset, length);
    }

    void get(void* buffer, const fam::size_t offset, const fam::size_t length) {
        session_->get(*object_, buffer, offset, length);
    }

    template<typename T>
    void set(const fam::size_t offset, const T value) {
        session_->set(*object_, offset, value);
    }

    template<typename T>
    auto fetch(const fam::size_t offset) -> T {
        return session_->fetch<T>(*object_, offset);
    }

    template<typename T>
    void add(const fam::size_t offset, const T value) {
        session_->add(*object_, offset, value);
    }

    template<typename T>
    void subtract(const fam::size_t offset, const T value) {
        session_->subtract(*object_, offset, value);
    }

    template<typename T>
    auto swap(const fam::size_t offset, const T value) -> T {
        return session_->swap(*object_, offset, value);
    }

    template<typename T>
    auto compareSwap(const fam::size_t offset, const T oldValue, const T newValue) -> T {
        return session_->compareSwap(*object_, offset, oldValue, newValue);
    }

    friend std::ostream& operator<<(std::ostream& out, const FamObjectDetail& object) {
        out << "name=" << object.name() << ", size=" << object.size() << ", permissions=" << object.permissions()
            << ", status=";
        switch (object.status()) {
            case Fam_Descriptor_Status::DESC_INVALID:                     out << "invalid"; break;
            case Fam_Descriptor_Status::DESC_INIT_DONE:                   out << "initialized"; break;
            case Fam_Descriptor_Status::DESC_INIT_DONE_BUT_KEY_NOT_VALID: out << "initialized_invalidkey"; break;
            case Fam_Descriptor_Status::DESC_UNINITIALIZED:               out << "uninitialized"; break;
            default:                                                      out << "unknown"; break;
        }
        return out;
    }

private:  // members
    std::shared_ptr<FamSessionDetail> session_;

    // openfam item descriptor
    std::unique_ptr<FamObjectDescriptor> object_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
