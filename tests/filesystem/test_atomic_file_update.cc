/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/log/JSON.h"
#include "eckit/parser/JSONParser.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Atomically update the contents of a file") {

    std::ostringstream s;
    JSON json(s);
    json.startObject();
    json << "restart" << true;
    json << "step" << int(42);
    json << "description"
         << "forecast run";
    json.endObject();

    Log::info() << s.str() << std::endl;

    PathName path = LocalPathName::cwd() + "/tmp/control.json";

    PathName tmppath = PathName::unique(path);

    Log::info() << path << std::endl;
    Log::info() << tmppath << " : exists " << tmppath.exists() << std::endl;

    std::ofstream of(tmppath.localPath());
    if (!of)
        throw CantOpenFile(tmppath.localPath(), Here());
    of << s.str();
    of.close();

    PathName::rename(tmppath, path);

    Log::info() << path << " : exists " << path.exists() << std::endl;

    Value j = JSONParser::decodeFile(path);

    EXPECT(j.isOrderedMap());

    EXPECT(j["restart"].isBool());
    EXPECT(j["restart"].as<bool>() == true);

    EXPECT(j["step"].isNumber());
    EXPECT(int(j["step"]) == 42);

    EXPECT(j["description"].isString());
    EXPECT(j["description"].as<string>() == std::string("forecast run"));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
