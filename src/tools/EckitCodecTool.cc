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


#include "EckitCodecTool.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"


namespace eckit::tools {


void EckitCodecTool::help(std::ostream& out) {
    auto indented = [&](const std::string& s) -> std::string {
        std::string str = indent() + s;

        for (size_t pos = 0; (pos = str.find('\n', pos)) != std::string::npos; ++pos) {
            str.replace(pos, 1, '\n' + indent());
        }

        return str;
    };

    out << "NAME\n" << indented(name());

    if (auto brief = briefDescription(); !brief.empty()) {
        out << " - " << brief << '\n';
    }

    if (auto usg = usage(); !usg.empty()) {
        out << "\nSYNOPSIS\n" << indented(usg) << '\n';
    }

    if (auto desc = longDescription(); !desc.empty()) {
        out << "\nDESCRIPTION\n" << indented(desc) << '\n';
    }

    out << "\nOPTIONS\n";
    for (const auto& option_ptr : options_) {
        std::stringstream s;
        s << *option_ptr;
        out << indented(s.str()) << "\n\n";
    }

    out << std::flush;
}


int EckitCodecTool::start() {
    try {
        if (handle_help()) {
            return success();
        }

        std::function<void(const std::string&)> dummy = [](const std::string&) {};
        Args args(dummy, options_, numberOfPositionalArguments(), minimumPositionalArguments());

        int err_code = execute(args);
        return err_code;
    }
    catch (Exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << "\n** Exception terminates " << name()
                     << std::endl;
    }
    catch (std::exception& e) {
        Log::error() << "** " << e.what() << " Caught in " << Here() << "\n** Exception terminates " << name()
                     << std::endl;
    }
    return failed();
}


bool EckitCodecTool::handle_help() {
    for (int i = 1; i < argc(); ++i) {
        if (argv(i) == "--help" || argv(i) == "-h") {
            help();
            return true;
        }
    }
    return false;
}


EckitCodecTool::EckitCodecTool(int argc, char** argv) : eckit::Tool(argc, argv) {
    add_option(new option::SimpleOption<bool>("help", "Print this help"));
}


}  // namespace eckit::tools
