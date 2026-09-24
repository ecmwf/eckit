// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamTypes.h"

#include <sstream>

#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

fam::index_t FamDescriptor::pack() const {
    if (region > fam::packed_region_max || offset > fam::packed_offset_mask) {
        std::ostringstream msg;
        msg << "FamDescriptor::pack: address {region=" << region << ", offset=" << offset << "} exceeds the "
            << (64U - fam::packed_offset_bits) << "/" << fam::packed_offset_bits << " bit split";
        throw SeriousBug(msg.str(), Here());
    }
    return (region << fam::packed_offset_bits) | offset;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
