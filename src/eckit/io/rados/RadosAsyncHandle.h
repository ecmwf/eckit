/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Nicolau Manubens
/// @date   March 2024

#pragma once

#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

/// @note: ensures writes are persisted on flush

class RadosAsyncHandle : public eckit::RadosHandle {

public:  // methods

    RadosAsyncHandle(const RadosObject&, size_t maxAioBuffSize = 1024 * 1024);

public:  // methods

    long write(const void*, long) override;
    void close() override;
    void flush() override;

    void print(std::ostream&) const override;

private:  // members

    std::vector<std::unique_ptr<eckit::RadosAIO>> comps_;
    size_t maxAioBuffSize_;

};

}  // namespace eckit
