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
public:  // methods
    constexpr Optional() noexcept :
        val_{None{}}, hasValue_(false){};

    // constexpr copy constructor only possible in C++14 because hasValue_ needs to be accessed conditionally and new (*ptr) should be used to initialize tho value
    explicit Optional(T&& v) :
        val_{None{}}, hasValue_(true) {
        new (&val_.some) T(std::move(v));
    }

    // constexpr copy constructor only possible in C++14 because hasValue_ needs to be accessed conditionally and new (*ptr) should be used to initialize tho value
    explicit Optional(const T& v) :
        val_{None{}}, hasValue_(true) {
        new (&val_.some) T(v);
    }

    // constexpr copy constructor only possible in C++14 because hasValue_ needs to be accessed conditionally and new (*ptr) should be used to initialize tho value
    Optional(const Optional<T>& rhs) :
        val_{None{}}, hasValue_(rhs.hasValue_) {
        if (hasValue_) {
            new (&val_.some) T(rhs.val_.some);
        }
    }

    // constexpr copy constructor only possible in C++14 because hasValue_ needs to be accessed conditionally and new (*ptr) should be used to initialize tho value
    Optional(Optional<T>&& rhs) :
        val_{None{}}, hasValue_(rhs.hasValue_) {
        if (hasValue_) {
            new (&val_.some) T(std::move(rhs.val_.some));
        }
        rhs.hasValue_ = false;
    }

    void destruct() {
        if (hasValue_) {
            val_.some.~T();
        }
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
        return val_.some;
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

private:  // members
    struct None {};
    union t_opt_value_type {
        None none;
        T some;
        ~t_opt_value_type() = default;
    };
    union nt_opt_value_type {
        None none;
        T some;
        ~nt_opt_value_type(){};
    };

    using opt_value_type = typename std::conditional<std::is_trivially_destructible<T>::value, t_opt_value_type, nt_opt_value_type>::type;

    opt_value_type val_;
    bool hasValue_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit
