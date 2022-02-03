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
public:

    Optional() : holder_(nullptr) {}
    ~Optional() { delete holder_; }

    Optional& operator= (T&& obj) {
        if (holder_)
            delete holder_;

        holder_ = new T(std::move(obj));

        return *this;
    }

    operator bool() const { return holder_ != nullptr; }

    T& value() { return *holder_; }
    T& operator*() { return *holder_; }

private:
    T* holder_;
};

} // end namespace eckit