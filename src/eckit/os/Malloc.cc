/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/eckit.h"

#include "eckit/os/Malloc.h"
#include "eckit/types/Types.h"

// TODO: use ecbuild macros instead
#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

unsigned long long Malloc::allocated() {
#ifdef __APPLE__
    return mstats().bytes_used;
#else
    return mallinfo().uordblks;
#endif
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

