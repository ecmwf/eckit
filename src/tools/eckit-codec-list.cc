/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <sstream>

#include "EckitCodecTool.h"

#include "eckit/codec/RecordPrinter.h"
#include "eckit/codec/print/Bytes.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"


namespace eckit::tools {


struct EckitCodecList : public EckitCodecTool {
    std::string briefDescription() override { return "Inspection of eckit::codec files"; }
    std::string usage() override { return name() + " <file> [OPTION]... [--help,-h]"; }
    std::string longDescription() override {
        return "Inspection of eckit::codec files\n"
               "\n"
               "       <file>: path to eckit::codec file";
    }

    EckitCodecList(int argc, char** argv) : EckitCodecTool(argc, argv) {
        add_option(new option::SimpleOption<std::string>("format", "Output format"));
        add_option(new option::SimpleOption<bool>("version", "Print version of records"));
        add_option(new option::SimpleOption<bool>("details", "Print detailed information"));
    }

    int execute(const Args& args) override {
        auto return_code = success();

        // User sanity checks
        if (args.count() == 0) {
            Log::error() << "No file specified." << std::endl;
            help(Log::error());
            return failed();
        }

        // Configuration
        eckit::LocalConfiguration config;
        config.set("format", args.getString("format", "table"));
        config.set("details", args.getBool("details", false));

        // Loop over files
        for (size_t f = 0; f < args.count(); ++f) {
            eckit::PathName file(args(f));
            if (!file.exists()) {
                Log::error() << "File does not exist: " << file << std::endl;
                return failed();
            }

            codec::Session session;

            std::uint64_t pos = 0;
            try {
                for (auto filesize = static_cast<size_t>(file.size()); pos < filesize;) {
                    auto uri    = codec::Record::URI{file, pos};
                    auto record = codec::RecordPrinter{uri, config};

                    std::stringstream out;
                    out << "\n# " << uri.path << " [" << uri.offset << "]    "
                        << "{ size: " << codec::Bytes{record.size()}.str(0) << ",    version: " << record.version()
                        << ",    created: " << record.time() << " }";
                    out << '\n' << (config.getString("format") == "table" ? "" : "---") << '\n';
                    out << record << std::endl;

                    std::cout << out.str();

                    pos += record.size();
                }
            }
            catch (const Exception& e) {
                Log::error() << "    ERROR: " << e.what() << std::endl;
                return_code = failed();
            }
        }
        return return_code;
    }
};


}  // namespace eckit::tools


int main(int argc, char** argv) {
    return eckit::tools::EckitCodecList{argc, argv}.start();
}
