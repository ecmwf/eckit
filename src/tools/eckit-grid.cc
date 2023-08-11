/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <memory>
#include <string>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/Grid.h"
#include "eckit/geometry/Point.h"
#include "eckit/log/JSON.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class EckitGrid final : public EckitTool {
public:
    EckitGrid(int argc, char** argv) :
        EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<bool>("list", "List possible grids"));
        options_.push_back(new option::SimpleOption<bool>("uid", "by grid unique identifier, instead of name"));
    }

private:
    void execute(const option::CmdArgs& args) override {
        bool list = false;
        args.get("list", list);

        bool uid = false;
        args.get("uid", uid);

        if (list) {
            geometry::GridFactory::list(Log::info());
            Log::info() << std::endl;
            return;
        }

        geometry::Point x = geometry::PointLonLat(0., 90.);
        std::cout << x << std::endl;

        auto& out = Log::info();
        // JSON out(Log::info());
        // out.precision(args.getInt("precision", 16));

        for (std::string arg : args) {
            std::unique_ptr<const geometry::Grid> grid(uid ? geometry::GridFactoryUID::build(arg) : geometry::GridFactoryName::build(arg));

            // out << "name: " << grid->name() << std::endl;
            // out << "uid: " << grid->uid() << std::endl;
            out << "size: " << grid->size() << std::endl;
            out << "bounding_box: " << grid->boundingBox() << std::endl;

            auto it = grid->begin();

            std::cout << **it << std::endl;

            //             (*it).print(out) << std::endl;
            //             static_cast<std::ostream&>(out) << "first: " << q << std::endl;

            // it += grid->size() - 1;
            // out << "last: " << **it << std::endl;
            // ASSERT(it == grid->rbegin());

            // out.startList();
            // for (const auto& p : *grid) {
            //     out << p;
            // }
            // out.endList();
        }
    }

    void usage(const std::string& tool) const override {
        Log::info() << "\n"
                       "Usage: "
                    << tool << "[options] ..." << std::endl;
    }

    int minimumPositionalArguments() const override {
        return 0;
    }
};

}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    eckit::EckitGrid app(argc, argv);
    return app.start();
}
