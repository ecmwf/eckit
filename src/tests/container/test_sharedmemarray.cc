/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE eckit_test_sharedmemarray

#include "ecbuild/boost_test_framework.h"

#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "eckit/types/Types.h"
#include "eckit/types/FixedString.h"
#include "eckit/container/SharedMemArray.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/utils/Translator.h"
#include "eckit/thread/AutoLock.h"

#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

struct ShmCleanup : Setup {
    ~ShmCleanup() {
        SYSCALL(::shm_unlink("/baz_hosts"));
    }
};

//----------------------------------------------------------------------------------------------------------------------

BOOST_GLOBAL_FIXTURE( ShmCleanup );

BOOST_AUTO_TEST_SUITE( test_eckit_sharedmemarray )

struct Host {
    eckit::FixedString<64> hostname_;
    size_t connetions_;
};

BOOST_AUTO_TEST_CASE( test_eckit_sharedmemarray_construction )
{
    SharedMemArray<Host> hosts("~/etc/baz/hosts", "/baz_hosts", 128);

    AutoLock< SharedMemArray<Host> > lock(hosts);

    hosts[0].hostname_   = std::string("calvin");
    hosts[0].connetions_ = 1;

    hosts[1].hostname_ = std::string("hobbes");
    hosts[1].connetions_ = 2;
}

BOOST_AUTO_TEST_CASE( test_eckit_sharedmemarray_checkvalues )
{
    SharedMemArray<Host> hosts("~/etc/baz/hosts", "/baz_hosts", 128);

    AutoLock< SharedMemArray<Host> > lock(hosts);

    BOOST_CHECK_EQUAL( hosts[0].hostname_ , std::string("calvin"));
    BOOST_CHECK_EQUAL( hosts[0].connetions_ , 1);

    BOOST_CHECK_EQUAL( hosts[1].hostname_ , std::string("hobbes"));
    BOOST_CHECK_EQUAL( hosts[1].connetions_ , 2);

    hosts[1].connetions_++;
}

BOOST_AUTO_TEST_CASE( test_eckit_sharedmemarray_add_more )
{
    SharedMemArray<Host> hosts("~/etc/baz/hosts", "/baz_hosts", 128);

    AutoLock< SharedMemArray<Host> > lock(hosts);

    BOOST_CHECK_EQUAL( hosts[1].hostname_ , std::string("hobbes"));
    BOOST_CHECK_EQUAL( hosts[1].connetions_ , 3);

    eckit::Translator<size_t,std::string> toStr;

    for(size_t i = 0; i < hosts.size(); ++i) {
        std::string h("node");
        h += toStr(i);
        hosts[i].hostname_ = h;
        hosts[i].connetions_ = 2*i;
    }
}

BOOST_AUTO_TEST_CASE( test_eckit_sharedmemarray_checkvalues_2 )
{
    SharedMemArray<Host> hosts("~/etc/baz/hosts", "/baz_hosts", 128);

    AutoLock< SharedMemArray<Host> > lock(hosts);

    eckit::Translator<size_t,std::string> toStr;

    for(size_t i = 0; i < hosts.size(); ++i) {
        std::string h("node");
        h += toStr(i);
        BOOST_CHECK_EQUAL( hosts[i].hostname_ , h);
        BOOST_CHECK_EQUAL( hosts[i].connetions_ , 2*i);
    }
}

BOOST_AUTO_TEST_SUITE_END()

//----------------------------------------------------------------------------------------------------------------------

} // namespace test
} // namespace eckit


