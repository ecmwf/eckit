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
/// @author Philipp Geier
///  - Added copy & move assignment operator (has been implicitly deleted  due to move constructor.)
/// @date   Feb 22

#pragma once

#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct None {};

constexpr None nullopt{};

// Define non-trivial or trivial destructor in template specialization
template <typename T>
class Optional;

// Define trivial destructor in specialized base class - allows having constexpr optional for trivial types
template <typename T, typename Enable = void>
class OptionalBase {
public:

    ~OptionalBase() { static_cast<Optional<T>*>(this)->reset(); }

protected:

    // Union with members that have non-trivial destructors need to define a destructor
    union opt_value_type {
        None none;
        T value;
        ~opt_value_type() {}
    };
};

template <typename T>
class OptionalBase<T, typename std::enable_if<std::is_trivially_destructible<T>::value>::type> {
public:

    ~OptionalBase() = default;

protected:

    // Using union instead of a char* array with alignas allows getting rid of reinterpret_cast (which might be
    // implementation dependent) and allows defining constexpr for trivial types An implementation in C++14 would also
    // use unions to have full constexpr support Union with members that have trivial destructors can have a default
    // destructor
    union opt_value_type {
        None none;
        T value;
        ~opt_value_type() = default;
    };
};

template <typename T>
class Optional : public OptionalBase<T> {
protected:

    struct TagValueConstructor {};
    struct TagCopyConstructor {};
    // struct TagMoveConstructor {};

    // Value constructor for trivial classes. No construction happening, can assign directly and hence make it a
    // constexpr
    template <typename TV, typename std::enable_if<(std::is_trivial<TV>::value), bool>::type = true>
    constexpr Optional(TagValueConstructor, TV&& v) : val_{.value = std::forward<TV>(v)}, hasValue_(true) {}

    // Value constructor for non-trivial classes - enforce construction with placement
    template <typename TV, typename std::enable_if<!(std::is_trivial<TV>::value), bool>::type = true>
    Optional(TagValueConstructor, TV&& v) : val_{None{}}, hasValue_(true) {
        new (&val_.value) T(std::forward<TV>(v));
    }

    // Copy constructor for trivial classes. No construction happening, can assign directly and hence make it a
    // constexpr
    template <typename TV, typename std::enable_if<(std::is_trivial<TV>::value), bool>::type = true>
    constexpr Optional(TagCopyConstructor, const TV& other) :
        val_{.value = other.val_.value}, hasValue_(other.hasValue_) {}

    // Copy constructor for non-trivial classes - enforce construction with placement
    template <typename TV, typename std::enable_if<!(std::is_trivial<TV>::value), bool>::type = true>
    Optional(TagCopyConstructor, const TV& other) : val_{None{}}, hasValue_(other.hasValue_) {
        if (hasValue_) {
            new (&val_.value) T(other.val_.value);
        }
    }

public:  // methods

    constexpr Optional() noexcept : val_{None{}}, hasValue_(false) {}

    constexpr explicit Optional(T&& v) noexcept : Optional(TagValueConstructor{}, std::move(v)) {}

    constexpr explicit Optional(const T& v) : Optional(TagValueConstructor{}, v) {}

    constexpr Optional(const Optional<T>& rhs) : Optional(TagCopyConstructor{}, rhs) {}

    ~Optional() = default;

    void reset() noexcept {
        if (hasValue_) {
            val_.value.~T();
            hasValue_ = false;
        }
    };

    // constexpr move constructor only possible in C++14 because hasValue_ needs to be accessed conditionally and new
    // (*ptr) should be used to initialize tho value Also trivial objects would need to modify rhs.hasValue_ to not
    // break semantics, which is only possible in C++14;
    Optional(Optional<T>&& rhs) noexcept : val_{None{}}, hasValue_(rhs.hasValue_) {
        if (hasValue_) {
            new (&val_.value) T(std::move(rhs.val_.value));
        }
        rhs.hasValue_ = false;
    }

    Optional<T>& operator=(const Optional<T>& other) {
        if (this == &other) {
            return *this;
        }

        if (hasValue_ && other.hasValue_) {
            val_.value = other.value();
        }
        else if (!hasValue_ && other.hasValue_) {
            // Explicitly construct here, previous value has been deleted.
            new (&val_.value) T(other.value());
            hasValue_ = true;
        }
        else if (hasValue_ && !other.hasValue_) {
            reset();
        }
        return *this;
    }

    Optional<T>& operator=(Optional<T>&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        if (hasValue_ && other.hasValue_) {
            val_.value = std::move(other.value());
        }
        else if (!hasValue_ && other.hasValue_) {
            // Explicitly construct here, previous value has been deleted.
            new (&val_.value) T(std::move(other.value()));
            hasValue_ = true;
        }
        else if (hasValue_ && !other.hasValue_) {
            reset();
        }
        return *this;
    }

    Optional<T>& operator=(const None&) {
        reset();
        return *this;
    }

    // The std::optional also seems to define a general assignment and does perfect forwarding.
    // This allows also passing other types from which the wrapped type T can be constructed or assigned
    template <typename U, typename enable = typename std::enable_if<
                              ((!std::is_same<typename std::decay<U>::type, Optional<T>>::value) &&
                               (std::is_constructible<T, U>::value) && (std::is_assignable<T&, U>::value))>::type>
    Optional<T>& operator=(U&& arg) {
        if (!hasValue_) {
            // Explicitly forward construct here, previous value has been deleted.
            new (&val_.value) T(std::forward<U>(arg));
        }
        else {
            // Can copy assign
            value() = std::forward<U>(arg);
        }
        hasValue_ = true;
        return *this;
    }

    // Force construct an value by forwarding arguments
    template <typename... Args>
    T& emplace(Args&&... args) {
        reset();
        new (&val_.value) T(std::forward<Args>(args)...);
        hasValue_ = true;
        return val_.value;
    }

    // Force construct an value by forwarding arguments
    template <class U, class... Args,
              typename enable =
                  typename std::enable_if<std::is_constructible<T, std::initializer_list<U>&, Args&&...>::value>::type>
    T& emplace(std::initializer_list<U> ilist, Args&&... args) {
        reset();
        new (&val_.value) T(ilist, std::forward<Args>(args)...);
        hasValue_ = true;
        return val_.value;
    }

    constexpr bool has_value() const { return hasValue_; }
    explicit constexpr operator bool() const { return has_value(); }

    const T& value() const& {
        if (!hasValue_) {
            throw eckit::Exception("Optional has no value.");
        }
        return val_.value;
    }
    T& value() & {
        if (!hasValue_) {
            throw eckit::Exception("Optional has no value.");
        }
        return val_.value;
    }
    T&& value() && {
        if (!hasValue_) {
            throw eckit::Exception("Optional has no value.");
        }
        return std::move(val_.value);
    }

    constexpr const T& operator*() const& { return val_.value; }
    T& operator*() & { return val_.value; }
    T&& operator*() && { return std::move(val_.value); }

    // TODO: Discuss about removing this in favour of * and -> (simialy to std::optional)
    constexpr const T& operator()() const& { return value(); }
    T& operator()() & { return value(); }
    T&& operator()() && { return value(); }

    constexpr const T* operator->() const& { return &val_.value; }
    T* operator->() & { return &val_.value; }

private:

    friend OptionalBase<T>;

    // While using ntd_opt_value_type would be just fine for both cases, doing the conditional type switch allows
    // creating constexpr for optional trivial types
    using opt_value_type = typename OptionalBase<T>::opt_value_type;

    opt_value_type val_;
    bool hasValue_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit
