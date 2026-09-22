// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <regex>

#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::tools {


class EckitFraction : public EckitTool {
public:

    EckitFraction(int argc, char** argv) : EckitTool(argc, argv) {}

private:

    void execute(const option::CmdArgs& args) override {
        const std::regex real_regex(R"([+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?)");

        Log::info().precision(16);
        Translator<std::string, double> to_double;

        for (const auto& arg : args) {
            Log::info() << (std::regex_match(arg, real_regex) ? std::to_string(to_double(arg)) : arg) << " = "
                        << Fraction{arg} << std::endl;
        }
    }

    void usage(const std::string& tool) const override {
        Log::info() << std::endl << "Usage: " << tool << "real [real [real]] ..." << std::endl;
        EckitTool::usage(tool);
    }

    int minimumPositionalArguments() const override { return 1; }
};


}  // namespace eckit::tools


int main(int argc, char** argv) {
    eckit::tools::EckitFraction app(argc, argv);
    return app.start();
}
