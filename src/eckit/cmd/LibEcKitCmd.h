// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Simon Smart
/// @date   May 2020

#ifndef eckit_cmd_LibEcKitCmd_H
#define eckit_cmd_LibEcKitCmd_H

#include "eckit/system/Library.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LibEcKitCmd : public eckit::system::Library {
    LibEcKitCmd();

public:  // methods

    static LibEcKitCmd& instance();

protected:  // methods

    const void* addr() const override;

    std::string version() const override;

    std::string gitsha1(unsigned int count) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
