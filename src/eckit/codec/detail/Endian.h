/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/eckit_config.h"

#ifndef eckit_BIG_ENDIAN
#error eckit_BIG_ENDIAN not defined
#endif

#ifndef eckit_LITTLE_ENDIAN
#error eckit_LITTLE_ENDIAN not defined
#endif

namespace eckit::codec {

enum class Endian {
    little = 0,
    big    = 1,
#if eckit_BIG_ENDIAN
    native  = big,
    swapped = little
#elif eckit_LITTLE_ENDIAN
    native  = little,
    swapped = big
#else
#error Neither eckit_BIG_ENDIAN nor eckit_LITTLE_ENDIAN equals true
#endif
};


}  // namespace eckit::codec
