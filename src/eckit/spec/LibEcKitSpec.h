// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/system/Library.h"


namespace eckit {


class LibEcKitSpec final : public system::Library {
public:

    // -- Methods

    static LibEcKitSpec& instance();

private:

    // -- Constructors

    LibEcKitSpec();

    // -- Overridden methods

    [[nodiscard]] const void* addr() const override;
    std::string version() const override;
    std::string gitsha1(unsigned int count) const override;
};


}  // namespace eckit
