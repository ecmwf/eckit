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

#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosObject.h"

namespace eckit {

class RadosAIO;

//----------------------------------------------------------------------------------------------------------------------

/// A RadosHandle whose writes are submitted without waiting for them.
/// They are only known to have been done, or to have failed, once flush() or close() returns.
class RadosAsyncHandle : public eckit::RadosHandle {

public:  // methods

    /// @param maxPendingBytes amount of data in writes not yet waited for, above which the next write flushes first
    explicit RadosAsyncHandle(const RadosObject& object, size_t maxPendingBytes = radosDefaultMaxPendingBytes);

    ~RadosAsyncHandle() override;

    void close() override;
    void flush() override;

    void print(std::ostream& out) const override;

protected:  // methods

    void submit(RadosWriteOp& op, long length) override;

private:  // members

    std::vector<std::unique_ptr<eckit::RadosAIO>> pending_;
    size_t pendingBytes_{0};
    size_t maxPendingBytes_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
