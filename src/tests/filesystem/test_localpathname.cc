/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_filesystem

#include <string>

#include "ecbuild/boost_test_framework.h"

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/FileSystemSize.h"

#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);


//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( TestLocalPathName )

BOOST_AUTO_TEST_CASE( test_constructors )
{
   BOOST_TEST_MESSAGE("eckit::filesystem:: ...test_constructors");

   LocalPathName p;
   BOOST_CHECK_MESSAGE(p == "/","Expected '/'");

   LocalPathName p1("/fred/bill");
   BOOST_CHECK_MESSAGE(p1 == "/fred/bill","Expected '/fred/bill' but found " << p);

   LocalPathName p2(p1);
   BOOST_CHECK_MESSAGE(p1 == p2,"Expected p1 == p2");
}

BOOST_AUTO_TEST_CASE( test_assignment )
{
   BOOST_TEST_MESSAGE("eckit::filesystem:: ...test_assignment");

   LocalPathName p;
   LocalPathName pd;
   p = pd;
   BOOST_CHECK_MESSAGE(p == "/","Expected '/'");

   LocalPathName p1("/fred/bill");
   LocalPathName p2;
   p2 = p1;
   BOOST_CHECK_MESSAGE(p2 == "/fred/bill","Expected '/fred/bill' but found " << p);

   LocalPathName p3;
   p3 = "/fred";
   BOOST_CHECK_MESSAGE(p3 == "/fred","Expected '/fred'");

   LocalPathName p4;
   p4 = std::string("/fredd");
   BOOST_CHECK_MESSAGE(p4 == "/fredd","Expected '/fredd'");
}

BOOST_AUTO_TEST_CASE( test_operators )
{
   BOOST_TEST_MESSAGE("eckit::filesystem:: ...test_operators");

   LocalPathName p;

   p += "fred";

   BOOST_CHECK_MESSAGE(p == "/fred","Expected '/fred'  but found " << p);

   p += "/joe/90";

   BOOST_CHECK_MESSAGE(p == "/fred/joe/90","Expected '/fred/joe/90'  but found " << p);

   p += '/';

   BOOST_CHECK_MESSAGE(p == "/fred/joe/90/","Expected '/fred/joe/90/'  but found " << p);
}


BOOST_AUTO_TEST_CASE( test_dir_name )
{
   BOOST_TEST_MESSAGE("eckit::filesystem:: ...test_dir_name");

   LocalPathName p("/fred/bill");
   BOOST_CHECK_MESSAGE(p.dirName() == "/fred","Expected dirName to be '/fred' but found " << p.dirName());
   BOOST_CHECK_MESSAGE(p.fullName() == "/fred/bill","Expected fullName to be '/fred/bill' but found " << p.fullName());

   // when no leading '/' on pathname, we append cwd to path, for fullName()
   LocalPathName p2("fred");
   LocalPathName expected = LocalPathName::cwd() + "/" + "fred";
   BOOST_CHECK_MESSAGE(p2.fullName() == expected,"Expected " << expected << " but found " << p2.fullName());
}

BOOST_AUTO_TEST_CASE( test_exists )
{
   BOOST_TEST_MESSAGE("eckit::filesystem:: ...test_exists");

   LocalPathName cwd = LocalPathName::cwd();

   BOOST_CHECK_MESSAGE(cwd.exists(),"Current working directory must exist");
   BOOST_CHECK_MESSAGE(cwd.isDir(),"Must be a directory");

   LocalPathName acwd = LocalPathName::cwd();
   BOOST_CHECK_MESSAGE(cwd.sameAs(acwd),"The directories must be the same");

   BOOST_TEST_MESSAGE("cwd " << cwd);
   BOOST_TEST_MESSAGE("cwd.mountPoint() " << cwd.mountPoint());
   BOOST_TEST_MESSAGE("cwd.realName() " << cwd.realName());
   BOOST_TEST_MESSAGE("cwd.node() " << cwd.node());
   BOOST_TEST_MESSAGE("cwd.path() " << cwd.path());
   BOOST_TEST_MESSAGE("size " << cwd.size());
   BOOST_TEST_MESSAGE("lastAccess " << cwd.lastAccess());
   BOOST_TEST_MESSAGE("lastModified " << cwd.lastModified());
   BOOST_TEST_MESSAGE("created " << cwd.created());
}


BOOST_AUTO_TEST_CASE( test_basename )
{
   BOOST_TEST_MESSAGE("eckit::filesystem:: ...test_basename");

   LocalPathName p1;
   BOOST_CHECK_MESSAGE(p1.baseName(false) == "","Expected empty string  but found " << p1.baseName(false));
   BOOST_CHECK_MESSAGE(p1.baseName(true) == "","Expected empty string  but found " << p1.baseName(true));

   LocalPathName p2("fred");
   BOOST_CHECK_MESSAGE(p2.baseName(false) == "fred","Expected 'fred' but found " << p2.baseName(false));
   BOOST_CHECK_MESSAGE(p2.baseName(true) == "fred","Expected 'fred' but found " << p2.baseName(true));

   LocalPathName p("/a/made/up/path/that/does/not/exist/file.ok");
   BOOST_CHECK_MESSAGE(p.baseName(false) == "file","Expected 'file'  but found " << p.baseName(false));
   BOOST_CHECK_MESSAGE(p.baseName(true) == "file.ok","Expected 'file.ok'  but found " << p.baseName(true));
}

BOOST_AUTO_TEST_CASE( test_extension )
{
   BOOST_TEST_MESSAGE("eckit::filesystem:: ...test_extension");

   LocalPathName p1;
   BOOST_CHECK_MESSAGE(p1.extension() == "", "Expected empty string  but found " << p1.extension());

   LocalPathName p2("fred");
   BOOST_CHECK_MESSAGE(p2.extension() == "", "Expected empty string  but found " << p2.extension());
   LocalPathName p3("/path/to/fred");
   BOOST_CHECK_MESSAGE(p3.extension() == "", "Expected empty string  but found " << p3.extension());
   LocalPathName p4("/path/with.dot/to/fred");
   BOOST_CHECK_MESSAGE(p4.extension() == "", "Expected empty string  but found " << p4.extension());

   LocalPathName p5("fred.");
   BOOST_CHECK_MESSAGE(p5.extension() == ".", "Expected '.'  but found " << p5.extension());
   LocalPathName p6("/path/to/fred.ext");
   BOOST_CHECK_MESSAGE(p6.extension() == ".ext", "Expected '.ext'  but found " << p6.extension());
   LocalPathName p7("/path/with.dot/to/fred.ext");
   BOOST_CHECK_MESSAGE(p7.extension() == ".ext", "Expected '.ext'  but found " << p7.extension());
}

BOOST_AUTO_TEST_CASE( test_fileSystemSize )
{
   BOOST_TEST_MESSAGE("eckit::filesystem:: ...test_fileSystemSize");

   LocalPathName cwd = LocalPathName::cwd();
   FileSystemSize fs;
   cwd.fileSystemSize(fs);

   BOOST_TEST_MESSAGE("cwd " << cwd);
   BOOST_TEST_MESSAGE("fs.available " << fs.available);
   BOOST_TEST_MESSAGE("fs.total " << fs.total);

   BOOST_CHECK_MESSAGE(fs.available > 0,"Expected file system available  > 0 ");
   BOOST_CHECK_MESSAGE(fs.total > 0,"Expected file system total  > 0 ");
}

BOOST_AUTO_TEST_CASE( test_unique )
{
   BOOST_TEST_MESSAGE("eckit::filesystem:: ...test_unique");

   LocalPathName unique = LocalPathName::unique(LocalPathName::cwd());

   unique.mkdir();

   BOOST_CHECK_MESSAGE(unique.exists(),"Expected '" << unique << "' to exist ");

   unique.rmdir();
   BOOST_CHECK_MESSAGE(!unique.exists(),"Expected '" << unique << "' to be removed ");
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
