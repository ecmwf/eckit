/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <cstdlib>

#define BOOST_TEST_MODULE test_eckit_system

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"

#include "eckit/system/ResourceUsage.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/system/Library.h"
#include "eckit/filesystem/LocalPathName.h"

#include "eckit/testing/Setup.h"

using namespace eckit;
using namespace eckit::testing;

//----------------------------------------------------------------------------------------------------------------------

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_resource_usage )

BOOST_AUTO_TEST_CASE( test_eckit_resource_usage_0 )
{
    size_t chunk = 20*1024*1024;
    for(size_t i = 1; i < 5; ++i) {

        size_t before = system::ResourceUsage().maxResidentSetSize();

        void *m = ::malloc(chunk);
        ::memset(m,0,chunk);

        size_t after = system::ResourceUsage().maxResidentSetSize();

        BOOST_TEST_MESSAGE( "Memory usage " << after );

        BOOST_REQUIRE( before <= after );
    }
}

BOOST_AUTO_TEST_CASE( test_eckit_system_info )
{
    eckit::LocalPathName execPath;
    BOOST_CHECK_NO_THROW( execPath = eckit::system::SystemInfo::instance().executablePath() );
    BOOST_CHECK( std::string(execPath).size() );

    eckit::Log::info() << "execPath is " << execPath << std::endl;
}

BOOST_AUTO_TEST_CASE( test_eckit_system_library )
{
    using eckit::system::Library;

    std::vector<std::string> libs = Library::list();

    std::string libpath;

    for(std::vector<std::string>::const_iterator libname = libs.begin(); libname != libs.end(); ++libname ) {

        BOOST_CHECK_NO_THROW( Library::lookup(*libname) );

        const Library& lib = Library::lookup(*libname);

        BOOST_CHECK_NO_THROW( lib.etcDirectory() );

        eckit::Log::info() << "Library " << lib.name() << " @ " << lib.etcDirectory() << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()

