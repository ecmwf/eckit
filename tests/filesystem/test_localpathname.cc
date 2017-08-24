/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/FileSystemSize.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

CASE( "test_constructors" )
{
   Log::info() << "eckit::filesystem:: ...test_constructors" << std::endl;

   LocalPathName p;
   EXPECT(p == "/");

   LocalPathName p1("/fred/bill");
   EXPECT(p1 == "/fred/bill");

   LocalPathName p2(p1);
   EXPECT(p1 == p2);
}

CASE( "test_assignment" )
{
   Log::info() << "eckit::filesystem:: ...test_assignment" << std::endl;

   LocalPathName p;
   LocalPathName pd;
   p = pd;
   EXPECT(p == "/");

   LocalPathName p1("/fred/bill");
   LocalPathName p2;
   p2 = p1;
   EXPECT(p2 == "/fred/bill");

   LocalPathName p3;
   p3 = "/fred";
   EXPECT(p3 == "/fred");

   LocalPathName p4;
   p4 = std::string("/fredd");
   EXPECT(p4 == "/fredd");
}

CASE( "test_operators" )
{
   Log::info() << "eckit::filesystem:: ...test_operators" << std::endl;

   LocalPathName p;

   p += "fred";

   EXPECT(p == "/fred");

   p += "/joe/90";

   EXPECT(p == "/fred/joe/90");

   p += '/';

   EXPECT(p == "/fred/joe/90/");
}


CASE( "test_dir_name" )
{
   Log::info() << "eckit::filesystem:: ...test_dir_name" << std::endl;

   LocalPathName p("/fred/bill");
   EXPECT(p.dirName() == "/fred");
   EXPECT(p.fullName() == "/fred/bill");

   // when no leading '/' on pathname, we append cwd to path, for fullName()
   LocalPathName p2("fred");
   LocalPathName expected = LocalPathName::cwd() + "/" + "fred";
   EXPECT(p2.fullName() == expected);
}

CASE( "test_exists" )
{
   Log::info() << "eckit::filesystem:: ...test_exists" << std::endl;

   LocalPathName cwd = LocalPathName::cwd();

   EXPECT(cwd.exists());
   EXPECT(cwd.isDir());

   LocalPathName acwd = LocalPathName::cwd();
   EXPECT(cwd.sameAs(acwd));

   Log::info() << "cwd " << cwd << std::endl;
   Log::info() << "cwd.mountPoint() " << cwd.mountPoint() << std::endl;
   Log::info() << "cwd.realName() " << cwd.realName() << std::endl;
   Log::info() << "cwd.node() " << cwd.node() << std::endl;
   Log::info() << "cwd.path() " << cwd.path() << std::endl;
   Log::info() << "size " << cwd.size() << std::endl;
   Log::info() << "lastAccess " << cwd.lastAccess() << std::endl;
   Log::info() << "lastModified " << cwd.lastModified() << std::endl;
   Log::info() << "created " << cwd.created() << std::endl;
}

CASE( "test_relative_path" )
{
    LocalPathName foobar ("/foo/bar/1/2");
    LocalPathName foozing("/foo/zing/3");
    LocalPathName baz("/baz/koko/4");
    LocalPathName root("/");

    // shorter relative to longer

    LocalPathName r1 = foozing.relativePath(foobar);

    EXPECT( r1 == LocalPathName("../../../zing/3") );

    // longer relative to shorter

    LocalPathName r2 = foobar.relativePath(foozing);

    EXPECT( r2 == LocalPathName("../../bar/1/2") );

    // same relative to same

    LocalPathName r3 = foobar.relativePath(foobar);

    EXPECT( r3 == LocalPathName(".") );

    // relative to root

    EXPECT( foobar.relativePath("/") == LocalPathName("foo/bar/1/2") );

    // root relative to path

    EXPECT( root.relativePath(foobar) == LocalPathName("../../../..") );
}

CASE( "test_children" )
{
    std::set<LocalPathName> reference;
    reference.insert("testdir/foo");
    reference.insert("testdir/foo/1");
    reference.insert("testdir/foo/2");
    reference.insert("testdir/foo/2/1");
    reference.insert("testdir/bar");
    reference.insert("testdir/baz");

    LocalPathName t = LocalPathName::cwd() + "/testdir";

    std::vector<LocalPathName> files;
    std::vector<LocalPathName> dirs;

    t.children(files, dirs);

    for(std::vector<LocalPathName>::const_iterator d = dirs.begin(); d != dirs.end(); ++d) {
        LocalPathName r = d->relativePath(LocalPathName::cwd());
//        std::cout << "relative path " << r << std::endl;
        EXPECT(reference.find(r) != reference.end());
    }
}

CASE( "test_basename" )
{
   Log::info() << "eckit::filesystem:: ...test_basename" << std::endl;

   LocalPathName p1;
   EXPECT(p1.baseName(false) == "");
   EXPECT(p1.baseName(true) == "");

   LocalPathName p2("fred");
   EXPECT(p2.baseName(false) == "fred");
   EXPECT(p2.baseName(true) == "fred");

   LocalPathName p("/a/made/up/path/that/does/not/exist/file.ok");
   EXPECT(p.baseName(false) == "file");
   EXPECT(p.baseName(true) == "file.ok");
}

CASE( "test_extension" )
{
   Log::info() << "eckit::filesystem:: ...test_extension" << std::endl;

   LocalPathName p1;
   EXPECT(p1.extension() == "");

   LocalPathName p2("fred");
   EXPECT(p2.extension() == "");
   LocalPathName p3("/path/to/fred");
   EXPECT(p3.extension() == "");
   LocalPathName p4("/path/with.dot/to/fred");
   EXPECT(p4.extension() == "");

   LocalPathName p5("fred.");
   EXPECT(p5.extension() == ".");
   LocalPathName p6("/path/to/fred.ext");
   EXPECT(p6.extension() == ".ext");
   LocalPathName p7("/path/with.dot/to/fred.ext");
   EXPECT(p7.extension() == ".ext");
}

CASE( "test_fileSystemSize" )
{
   Log::info() << "eckit::filesystem:: ...test_fileSystemSize" << std::endl;

   LocalPathName cwd = LocalPathName::cwd();
   FileSystemSize fs;
   cwd.fileSystemSize(fs);

   Log::info() << "cwd " << cwd << std::endl;
   Log::info() << "fs.available " << fs.available << std::endl;
   Log::info() << "fs.total " << fs.total << std::endl;

   EXPECT(fs.available > 0);
   EXPECT(fs.total > 0);
}

CASE( "test_unique" )
{
   Log::info() << "eckit::filesystem:: ...test_unique" << std::endl;

   LocalPathName unique = LocalPathName::unique(LocalPathName::cwd());

   unique.mkdir();

   EXPECT(unique.exists());

   unique.rmdir();
   EXPECT(!unique.exists());
}

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv)
{
    return run_tests ( argc, argv );
}

