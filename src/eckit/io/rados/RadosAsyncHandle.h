// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date   March 2024

#pragma once

#include <cstddef>
#include <memory>
#include <ostream>
#include <vector>

#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosAsyncHandle : public eckit::RadosHandle {

public:  // methods

    explicit RadosAsyncHandle(const RadosObject& object, size_t maxAioBuffSize = 1024 * 1024);

public:  // methods

    long write(const void* buffer, long length) override;
    void close() override;
    void flush() override;

    void print(std::ostream& out) const override;

private:  // members

    std::vector<std::unique_ptr<eckit::RadosAIO>> comps_;
    size_t maxAioBuffSize_;  ///< bytes of in-flight writes above which write() flushes first
    size_t pendingBytes_{0};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
