/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <iostream>
#include <string>

#include "eckit/eckit_config.h"
#include "eckit/io/URLHandle.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

#ifdef ECKIT_HAVE_CURL

CASE( "test_urlhandle_get" )
{
    URLHandle h("https://www.ecmwf.int/");
    h.saveInto("/tmp/test_urlhandle_get.html");
}

CASE( "test_urlhandle_redirect" )
{
    URLHandle h("http://www.ecmwf.int");
    h.saveInto("/tmp/test_urlhandle_get.html");
}

CASE( "test_urlhandle_404" )
{
    // TODO: catch URLHandle::URLException and check code
    // URLHandle h("http://download.ecmwf.org/foobar");
    // h.saveInto("/tmp/test_urlhandle_get.html");
}
#else
CASE( "test_urlhandle" )
{
}
#endif

//-----------------------------------------------------------------------------

} // namespace test
} // namespace eckit

int main(int argc,char **argv)
{
    return run_tests ( argc, argv );
}


