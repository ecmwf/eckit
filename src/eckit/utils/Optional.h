/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

namespace eckit {

template <typename T>
class Optional {

public: // methods
    constexpr Optional() noexcept : valid_(false) {};

    explicit Optional(T&& v) : valid_(true) {
        new (val_) T(std::forward<T>(v));
    }

    Optional (const Optional<T>& rhs) : valid_(rhs.valid) {
        if (valid_)
            new (val_) T(*reinterpret_cast<const T*>(&rhs.val_));
    }

    Optional (Optional<T>&& rhs) : valid_(rhs.valid_) {
        if (valid_)
            new (val_) T(std::move(*reinterpret_cast<T*>(&rhs.val_)));
        rhs.valid_ = false;
    }

    Optional<T>& operator=(T&& v) {
        valid_ = true;
        new (val_) T(std::forward<T>(v));
        return *this;
    }

    ~Optional() {
        if (valid_) reinterpret_cast<T*>(&val_)->~T();
    }

    bool has_value() const {
        return valid_;
    }

    explicit operator bool() const {
        return has_value();
    }

    T& value() {
        return *value_ptr();
    }

    T* operator->() {
        return value_ptr();
    }

    T& operator*() {
         return value();
    }


private: // members
    T* value_ptr() {
        return reinterpret_cast<T*>(&val_);
    }

    alignas(T) char val_[sizeof(T)];
    bool valid_;
};

} // end namespace eckit