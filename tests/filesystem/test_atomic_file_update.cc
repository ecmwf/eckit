// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

namespace eckit::test {

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
    if (!of) {
        throw CantOpenFile(tmppath.localPath(), Here());
    }
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

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
