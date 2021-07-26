/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/config/LibEcKit.h"
#include "eckit/eckit_version.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/system/Library.h"
#include "eckit/system/LibraryManager.h"

using namespace eckit::system;

//----------------------------------------------------------------------------------------------------------------------

class EckitInfo : public eckit::EckitTool {

public:  // methods
    EckitInfo(int argc, char** argv) :
        eckit::EckitTool(argc, argv) {

        options_.push_back(new eckit::option::SimpleOption<bool>("json", "Produce JSON output"));

        options_.push_back(new eckit::option::SimpleOption<bool>("libraries", "Print loaded libraries"));
        options_.push_back(new eckit::option::SimpleOption<bool>("loaded-plugins", "Print loaded plugins"));
        options_.push_back(new eckit::option::SimpleOption<bool>("available-plugins", "Discover available plugins"));

        options_.push_back(new eckit::option::SimpleOption<bool>("version", "Print version number"));
    }

private:  // methods
    virtual void execute(const eckit::option::CmdArgs& args);
    virtual void usage(const std::string& tool) const;
    virtual int minimumPositionalArguments() const {
        return 0;
    }
};

void EckitInfo::usage(const std::string& tool) const {
    eckit::Log::info() << std::endl
                       << "Usage: " << tool << "[options] ..." << std::endl;
    eckit::EckitTool::usage(tool);
}

void EckitInfo::execute(const eckit::option::CmdArgs& args) {
    bool ojson = false;
    bool b;

    args.get("json", ojson);
    if (ojson) {
        eckit::JSON json(std::cout);

        json.startObject();

        b = false;
        args.get("version", b);
        if (b) {
            json << "version" << eckit_version_str();
        }

        b = false;
        args.get("libraries", b);
        if (b) {
            json << "libraries";
            json.startList();
            for (const std::string& libname : LibraryManager::list()) {
                const Library& lib = LibraryManager::lookup(libname);
                json.startObject();
                json << "name" << lib.name();
                json << "version" << lib.version();
                json << "sha1" << lib.gitsha1();
                json << "path" << lib.libraryPath();
                json.endObject();
            }
            json.endList();
        }

        b = false;
        args.get("loaded-plugins", b);
        if (b) {
            json << "loaded-plugins";
            json.startList();
            for (auto plugin : LibraryManager::loadedPlugins()) {
                json << plugin;
            }
            json.endList();
        }

        json.endObject();
    }
    else {
        b = false;
        args.get("version", b);
        if (b) {
            std::cout << "eckit version " << eckit_version_str() << std::endl;
        }

        b = false;
        args.get("libraries", b);
        if (b) {
            std::cout << "Libraries:" << std::endl;
            for (const std::string& libname : LibraryManager::list()) {
                const Library& lib = LibraryManager::lookup(libname);
                std::cout << "  "
                          << lib.name() << " "
                          << lib.version() << " "
                          << "(" << lib.gitsha1(7) << ") "
                          << lib.libraryPath() << std::endl;
            }
        }

        b = false;
        args.get("loaded-plugins", b);
        if (b) {
            std::cout << "Loaded plugins: " << std::endl;
            for (auto plugin : LibraryManager::loadedPlugins()) {
                std::cout << "  " << plugin << std::endl;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    EckitInfo app(argc, argv);
    return app.start();
}
