/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"

using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    /* log before context build */

    Log::info() << "logging before calling Context" << std::endl;

    return 0;
}
