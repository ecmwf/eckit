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

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Helper class to manage an optional contained value,
/// i.e. a value that may or may not be present.

// Define non-trivial or trivial destructor in template specialization
template <typename T>
class Optional;

template <typename T, class Enable = void>
struct OptionalBase {
    ~OptionalBase() {
        static_cast<Optional<T>*>(this)->destruct();
    }
};

template <typename T>
struct OptionalBase<T, typename std::enable_if<std::is_trivially_destructible<T>::value>::type> {
};

template <typename T>
class Optional : public OptionalBase<T> {
protected:
    struct TagValueConstructor {};
    struct TagCopyConstructor {};
    // struct TagMoveConstructor {};

    // Value constructor for trivial classes. No construction happening, can assign directly and hence make it a constexpr
    template <typename TV, typename std::enable_if<(std::is_trivial<TV>::value), bool>::type = true>
    constexpr Optional(TagValueConstructor, TV&& v) :
        val_{.some = std::forward<TV>(v)}, hasValue_(true) {}

    // Value constructor for non-trivial classes - enforce construction with placement
    template <typename TV, typename std::enable_if<!(std::is_trivial<TV>::value), bool>::type = true>
    Optional(TagValueConstructor, TV&& v) :
        val_{None{}}, hasValue_(true) {
        new (&val_.some) T(std::forward<TV>(v));
    }


    // Copy constructor for trivial classes. No construction happening, can assign directly and hence make it a constexpr
    template <typename TV, typename std::enable_if<(std::is_trivial<TV>::value), bool>::type = true>
    constexpr Optional(TagCopyConstructor, const TV& other) :
        val_{.some = other.val_.some}, hasValue_(other.hasValue_) {}

    // Copy constructor for non-trivial classes - enforce construction with placement
    template <typename TV, typename std::enable_if<!(std::is_trivial<TV>::value), bool>::type = true>
    Optional(TagCopyConstructor, const TV& other) :
        val_{None{}}, hasValue_(other.hasValue_) {
        if (hasValue_) {
            new (&val_.some) T(other.val_.some);
        }
    }

public:  // methods
    constexpr Optional() noexcept :
        val_{None{}}, hasValue_(false){};

    constexpr explicit Optional(T&& v) :
        Optional(TagValueConstructor{}, std::move(v)) {}

    constexpr explicit Optional(const T& v) :
        Optional(TagValueConstructor{}, v) {}

    constexpr Optional(const Optional<T>& rhs) :
        Optional(TagCopyConstructor{}, rhs) {}


    // constexpr move constructor only possible in C++14 because hasValue_ needs to be accessed conditionally and new (*ptr) should be used to initialize tho value
    // Also trivial objects would need to modify rhs.hasValue_ to not break semantics, which is only possible in C++14;
    Optional(Optional<T>&& rhs) :
        val_{None{}}, hasValue_(rhs.hasValue_) {
        if (hasValue_) {
            new (&val_.some) T(std::move(rhs.val_.some));
        }
        rhs.hasValue_ = false;
    }

    Optional<T>& operator=(const Optional<T>& other) {
        if (hasValue_ && other.hasValue_) {
            value() = other.value();
        }
        else if (!hasValue_ && other.hasValue_) {
            // Explicitly construct here, previous value has been deleted.
            new (&val_.some) T(other.value());
            hasValue_ = true;
        }
        else if (hasValue_ && !other.hasValue_) {
            val_.some.~T();
            hasValue_ = false;
        }
        return *this;
    }

    Optional<T>& operator=(Optional<T>&& other) {
        if (hasValue_ && other.hasValue_) {
            value()         = std::move(other.value());
            other.hasValue_ = false;
        }
        else if (!hasValue_ && other.hasValue_) {
            // Explicitly construct here, previous value has been deleted.
            new (&val_.some) T(std::move(other.value()));
            other.hasValue_ = false;
            hasValue_       = true;
        }
        else if (hasValue_ && !other.hasValue_) {
            val_.some.~T();
            hasValue_ = false;
        }
        return *this;
    }

    template <typename TV>
    Optional<T>& assignValue(TV&& v) {
        if (!hasValue_) {
            // Explicitly construct here, previous value has been deleted.
            new (&val_.some) T(std::forward<TV>(v));
        }
        else {
            // Can copy assign
            value() = std::forward<TV>(v);
        }
        hasValue_ = true;
        return *this;
    }

    Optional<T>& operator=(const T& v) {
        return assignValue(v);
    }
    Optional<T>& operator=(T&& v) {
        return assignValue(std::move(v));
    }

    constexpr bool has_value() const {
        return hasValue_;
    }
    explicit constexpr operator bool() const {
        return has_value();
    }

    constexpr const T& value() const& {
        return val_.some;
    }
    T& value() & {
        return val_.some;
    }
    T&& value() && {
        return std::move(val_.some);
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

protected:
    void destruct() {
        if (hasValue_) {
            val_.some.~T();
        }
    }

    friend OptionalBase<T>;

private:  // members
    struct None {};
    union td_opt_value_type {
        None none;
        T some;
        ~td_opt_value_type() = default;
    };
    union ntd_opt_value_type {
        None none;
        T some;
        ~ntd_opt_value_type(){};
    };

    using opt_value_type = typename std::conditional<std::is_trivially_destructible<T>::value, td_opt_value_type, ntd_opt_value_type>::type;

    opt_value_type val_;
    bool hasValue_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit
