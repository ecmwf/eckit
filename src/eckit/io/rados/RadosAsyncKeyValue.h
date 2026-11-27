// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date   June 2024

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosKeyValue.h"

namespace eckit {

class RadosAIO;

constexpr size_t radosDefaultMaxPendingOps = 1024;

//----------------------------------------------------------------------------------------------------------------------

/// A key-value whose modifications are submitted without waiting for them.
/// They are only known to have been applied, or to have failed, once flush() returns.
/// What was not flushed is waited for when the object is destroyed, where a failure can only be logged.
class RadosAsyncKeyValue : public RadosKeyValue {

public:  // methods

    /// @param maxPendingOps number of modifications not yet waited for, above which the next one flushes first
    explicit RadosAsyncKeyValue(const eckit::URI& uri, size_t maxPendingOps = radosDefaultMaxPendingOps);

    RadosAsyncKeyValue(const std::string& pool, const std::string& nspace, const std::string& name,
                       size_t maxPendingOps = radosDefaultMaxPendingOps);

    ~RadosAsyncKeyValue() override;

    RadosAsyncKeyValue(const RadosAsyncKeyValue&)            = delete;
    RadosAsyncKeyValue& operator=(const RadosAsyncKeyValue&) = delete;

    /// Waits for the modifications made so far.
    /// @throws the first failure among them, once all have been waited for
    void flush();

protected:  // methods

    void submit(RadosWriteOp& op) override;

private:  // members

    std::vector<std::unique_ptr<eckit::RadosAIO>> pending_;
    size_t maxPendingOps_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
