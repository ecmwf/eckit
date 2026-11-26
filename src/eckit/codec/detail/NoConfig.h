// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/config/LocalConfiguration.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

class NoConfig : public LocalConfiguration {
public:

    NoConfig()          = default;
    virtual ~NoConfig() = default;
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
