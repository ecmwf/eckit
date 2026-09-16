// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/codec/detail/DataType.h"

#include <sstream>

#include "eckit/codec/Exceptions.h"

//------------------------------------------------------------------------------------------------------

namespace eckit::codec {

void DataType::throw_not_recognised(kind_t kind) {
    std::stringstream msg;
    msg << "kind [" << kind << "] not recognised.";
    throw Exception(msg.str(), Here());
}

void DataType::throw_not_recognised(std::string datatype) {
    std::stringstream msg;
    msg << "datatype [" << datatype << "] not recognised.";
    throw Exception(msg.str(), Here());
}


//------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
