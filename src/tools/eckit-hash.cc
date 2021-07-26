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
#include <memory>
#include <string>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/log/Plural.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Hash.h"

using eckit::DataHandle;
using eckit::Hash;
using eckit::Length;
using eckit::LibEcKit;
using eckit::Log;
using eckit::PathName;

const char* defaultHashType = "xxh64";

class HashTool : public eckit::Tool {
public:
    HashTool(int argc, char** argv) :
        Tool(argc, argv) {
        options_.push_back(new eckit::option::SimpleOption<std::string>("type", "hash type, default=" + std::string(defaultHashType)));
        options_.push_back(new eckit::option::SimpleOption<bool>("quiet", "silent mode with less output, only errors on check, default false"));
        options_.push_back(new eckit::option::SimpleOption<bool>("continue", "continues on failed comparisons, default true"));
        options_.push_back(new eckit::option::SimpleOption<bool>("recurse", "recurse into sub-dirs, default true"));
        options_.push_back(new eckit::option::SimpleOption<bool>("check", "compare hash of all files found, default true"));
        options_.push_back(new eckit::option::SimpleOption<bool>("generate", "generate a hash for all files found, default false"));
        options_.push_back(new eckit::option::SimpleOption<bool>("clean", "cleans hash files for original files that no longer exist, default false"));
    }

    virtual void run();

    void scan(PathName& path);
    void hash(PathName& path);

    Hash::digest_t computeHash(PathName& path);

    std::vector<eckit::option::Option*> options_;
    std::set<std::string> visited_;

    std::string type_;
    std::string extension_;

    std::vector<std::string> failures_;

    bool quiet_;
    bool continue_;
    bool recurse_;
    bool check_;
    bool generate_;
    bool clean_;

    std::unique_ptr<Hash> hash_;
    PathName last_;
};

static void usage(const std::string& tool) {
    Log::info() << "Usage:\n"
                << "  As scanner:   " << tool << "                        <path> [paths...]\n"
                << "  As generator: " << tool << " --check=0 --generate=1 <path> [paths...]\n"
                << "  As both:      " << tool << " --check=1 --generate=1 <path> [paths...]\n"
                << std::endl;
}

Hash::digest_t HashTool::computeHash(PathName& file) {
    if (last_ == file)  // avoid 2 hashes in check following a generate
        return hash_->digest();

    LOG_DEBUG_LIB(LibEcKit) << "Calculating hash for " << file << std::endl;
    hash_->reset();  // zero hash
    file.hash(*hash_);
    last_ = file;
    return hash_->digest();
}

void HashTool::hash(PathName& path) {

    LOG_DEBUG_LIB(LibEcKit) << "scanning path " << path << std::endl;

    if (not check_ and not generate_)
        return;

    // skip if this is a hash file
    if (path.extension() == extension_) {
        if (clean_) {  // remove hash file if original file was removed
            std::string s   = path.asString();
            PathName origin = s.substr(0, s.find_last_of('.'));
            if (not origin.exists()) {
                LOG_DEBUG_LIB(LibEcKit) << "origin file missing " << origin << std::endl;
                path.unlink(!quiet_);
            }
        }
        return;
    }

    PathName file     = path.realName();
    PathName hashpath = file + extension_;
    Hash::digest_t outdigest;

    LOG_DEBUG_LIB(LibEcKit) << "hash file " << hashpath << std::endl;

    if (generate_) {
        LOG_DEBUG_LIB(LibEcKit) << "generating " << file << std::endl;
        if (not hashpath.exists()) {
            Hash::digest_t digest = computeHash(file);
            outdigest             = digest;
            std::ofstream out;
            out.open(hashpath.asString().c_str(), std::ios::out | std::ios::trunc);
            if (not out) {
                throw eckit::WriteError(hashpath, Here());
            }
            out << digest;
            out.close();
        }
    }

    if (check_) {
        LOG_DEBUG_LIB(LibEcKit) << "checking " << file << std::endl;
        Hash::digest_t digest = computeHash(file);
        outdigest             = digest;
        if (hashpath.exists()) {
            std::ifstream in;
            in.open(hashpath.asString().c_str(), std::ios::in);
            if (not in) {
                throw eckit::ReadError(hashpath, Here());
            }
            Hash::digest_t readDigest;
            in >> readDigest;
            in.close();

            LOG_DEBUG_LIB(LibEcKit) << readDigest << " : " << digest << path << std::endl;

            if (readDigest != digest) {
                failures_.push_back(file.asString());
                Log::info() << "FAIL " << readDigest << " != " << digest << " : " << file << std::endl;
                if (not continue_) {
                    throw eckit::Stop("Hash check failed on " + file.asString());
                }
            }
        }
    }

    if (not quiet_)
        Log::info() << outdigest << " : " << file << std::endl;
}

void HashTool::scan(PathName& path) {

    if (path.isLink()) {
        return;
    }

    if (path.isDir()) {

        PathName realpath         = path.realName();
        const std::string pathstr = realpath.asString();

        if (visited_.find(pathstr) != visited_.end())
            return;

        visited_.insert(pathstr);

        // Log::info() << "Scanning path " << pathstr << std::endl;

        std::vector<PathName> files;
        std::vector<PathName> dirs;
        realpath.children(files, dirs);

        // process files in this directory
        for (auto& file : files) {
            hash(file);
        }

        // descend to children directory
        if (recurse_)
            for (auto& dir : dirs) {
                scan(dir);
            }
    }
    else {
        hash(path);
    }
}

void HashTool::run() {

    eckit::option::CmdArgs args(&usage, options_, -1, 1);

    type_      = args.getString("type", defaultHashType);
    extension_ = "." + type_;

    hash_.reset(eckit::HashFactory::instance().build(type_));

    quiet_    = args.getBool("quiet", false);
    continue_ = args.getBool("continue", true);
    recurse_  = args.getBool("recurse", true);
    check_    = args.getBool("check", true);
    generate_ = args.getBool("generate", false);
    clean_    = args.getBool("clean", false);

    try {
        for (size_t i = 0; i < args.count(); ++i) {
            PathName path(args(i));
            scan(path);
        }
    }
    catch (eckit::Stop& e) {
        Log::info() << e.what() << std::endl;
    }

    if (failures_.size()) {
        Log::info() << eckit::Plural(failures_.size(), "hash check") << " failed:" << std::endl;
        for (auto& f : failures_) {
            Log::info() << f << std::endl;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    HashTool tool(argc, argv);
    return tool.start();
}
