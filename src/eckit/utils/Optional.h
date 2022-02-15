/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Emanuele Danovaro
/// @author Simon Smart
/// @date   Feb 22

#pragma once

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Helper class to manage an optional contained value,
/// i.e. a value that may or may not be present.

template <typename T>
class Optional {

public: // methods
    constexpr Optional() noexcept : valid_(false) {};
    explicit Optional(T&& v) : valid_(true) {
        new (val_) T(std::forward<T>(v));
    }

    Optional(const Optional<T>& rhs) : valid_(rhs.valid) {
        if (valid_) {
            new (val_) T(*reinterpret_cast<const T*>(&rhs.val_));
        }
    }
    Optional(Optional<T>&& rhs) : valid_(rhs.valid_) {
        if (valid_) {
            new (val_) T(std::move(*reinterpret_cast<T*>(&rhs.val_)));
        }
        rhs.valid_ = false;
    }

    ~Optional() {
        if (valid_) {
            reinterpret_cast<T*>(&val_)->~T();
        }
    }

    Optional<T>& operator=(T&& v) {
        valid_ = true;
        new (val_) T(std::forward<T>(v));
        return *this;
    }

    bool has_value() const {
        return valid_;
    }
    explicit operator bool() const {
        return has_value();
    }

    constexpr const T& value() const& {
        return *const_ptr();
    }
    T& value() & {
        return *ptr();
    }
    T&& value() && {
        return *ptr();
    }

    constexpr const T& operator*() const& {
        return value();
    }
    T& operator*() & {
        return value();
    }
    T&& operator*() && {
        return value();
    }

    constexpr const T& operator()() const& {
        return value();
    }
    T& operator()() & {
        return value();
    }
    T&& operator()() && {
        return value();
    }

private: // members
    T* ptr() {
        return reinterpret_cast<T*>(&val_);
    }
    const T* const_ptr() const {
        return reinterpret_cast<const T*>(&val_);
    }

    alignas(T) char val_[sizeof(T)];
    bool valid_;
};

//----------------------------------------------------------------------------------------------------------------------

} // end namespace eckit