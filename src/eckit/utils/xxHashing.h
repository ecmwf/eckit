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

/// @note This file is named HashxxHash and not simply xxHash not to clash with the included header xxhash.h
///       for case insensitive file systems

#include <memory>

#include "eckit/eckit.h"
#include "eckit/utils/Hash.h"

namespace eckit {

class xxHash : public Hash {

public:  // types

    xxHash();

    explicit xxHash(const char*);
    explicit xxHash(const std::string&);

    xxHash(const void* data, size_t len);

    ~xxHash() override;

    void reset() const override;

    digest_t compute(const void*, long) override;

    void update(const void*, long) override;

    digest_t digest() const override;

    template <class T>
    xxHash& operator<<(const T& x) {
        add(x);
        return *this;
    }

private:  // members

    struct Context;
    std::unique_ptr<Context> ctx_;
};

}  // end namespace eckit
