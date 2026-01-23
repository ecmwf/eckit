/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstddef>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/utils/Translator.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class EckitGaussianLatitudes final : public EckitTool {
public:

    EckitGaussianLatitudes(int argc, char** argv) : EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<bool>("first", "Only the first latitude"));
    }

private:

    void execute(const option::CmdArgs& args) override {
        auto first = args.getBool("first", false);
        Translator<std::string, size_t> to_size_t;

        auto& out = Log::info();
        out.precision(16);

        for (const auto& N : args) {
            const auto& lats = geo::util::gaussian_latitudes(to_size_t(N), !first);
            ASSERT(!lats.empty());

            if (first) {
                out << lats.front() << std::endl;
                continue;
            }

            for (auto lat : lats) {
                out << lat << std::endl;
            }
        }
    }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "[options] N [N[...]]" << std::endl;
    }

    int minimumPositionalArguments() const override { return 0; }
};

}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    eckit::EckitGaussianLatitudes app(argc, argv);
    return app.start();
}
