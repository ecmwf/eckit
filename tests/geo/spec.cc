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


#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("user -> type") {
    using v = std::vector<double>;

    static const std::string BAD;
    ASSERT(BAD.empty());

    static std::pair<spec::Custom::container_type, std::string> tests[]{
        {{{"N", 2}}, "reduced_gg"},
        {{{"area", v{90, -180, -90, 180}}, {"grid", v{2, 2}}}, "regular_ll"},
        {{{"area", v{90, -180, -90, 180}}}, BAD},
        {{{"grid", "B48"}}, BAD},
        {{{"grid", "F48"}}, "regular_gg"},
        {{{"grid", "N48"}}, "reduced_gg"},
        {{{"grid", "O48"}}, "reduced_gg"},
        {{{"grid", 48}}, BAD},
        {{{"grid", v{2, 2}}}, "regular_ll"},
        {{{"grid", v{2, 2}}}, "regular_ll"},
        {{{"grid", "48"}}, BAD},
        {{{"grid", "F048"}}, BAD},
        {{{"grid", "N"}}, BAD},
        {{{"grid", "N048"}}, BAD},
        {{{"grid", "N48"}}, "reduced_gg"},
        {{{"grid", "O048"}}, BAD},
        {{{"grid", "O48"}}, "reduced_gg"},
        {{{"type", "reduced_gg"}}, ""},
        {{{"grid", 2}}, ""},
        {{{"grid", 12}}, ""},
        {{{"type", "regular_gg"}, {"grid", "48"}}, BAD},
        {{{"type", "regular_gg"}, {"grid", "F048"}}, BAD},
        {{{"type", "regular_gg"}, {"grid", "F48"}}, "regular_gg"},
        {{{"type", "regular_gg"}, {"grid", "N48"}}, "reduced_gg"},
        {{{"type", "regular_gg"}, {"grid", "O48"}}, "reduced_gg"},
        {{{"type", "regular_gg"}, {"grid", "a"}}, BAD},
        {{{"type", "regular_gg"}, {"grid", 48}}, BAD},
        {{{"type", "regular_ll"}, {"area", v{90, -180, -90, 180}}}, BAD},
        {{{"type", "regular_ll"}, {"grid", "F48"}}, BAD},
        {{{"type", "regular_ll"}, {"grid", "a"}}, BAD},
        {{{"type", "regular_ll"}, {"grid", std::vector<std::string>{"a", "b"}}}, BAD},
        {{{"type", "regular_ll"}, {"grid", v{1, 2, 3}}}, BAD},
        {{{"type", "regular_ll"}, {"grid", v{1, 2}}}, "regular_ll"},
        {{{"type", "regular_ll"}, {"grid", v{1}}}, BAD},

        {{{"type", "mercator"},
          {"area", v{31.173058, 262.036499, 14.736453, 284.975281}},
          {"grid", v{45000.0, 45000.0}},
          {"shape", std::vector<size_t>{56, 44}},
          {"lad", 14.0},
          {"orientation", 0.0}},
         BAD},
    };

    for (const auto& [user, ref] : tests) {
        spec::Custom userspec(user);

        Log::info() << userspec << " -> type: " << ref << std::endl;

        try {
            std::unique_ptr<const Spec> spec(GridFactory::make_spec(userspec));
            EXPECT(spec);

            std::unique_ptr<const Grid> grid(GridFactory::build(*spec));
            EXPECT(grid);
        }
        catch (const exception::SpecError& e) {
            EXPECT(ref == BAD);
        }
        catch (const BadParameter& e) {
            EXPECT(ref == BAD);
        }
    }
}


CASE("grid: name -> spec -> grid: name") {
// FIXME
#if 0
    for (const std::string& name : {"LAEA-EFAS-5km", "SMUFF-OPERA-2km"}) {
        std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({{"grid", name}})));
        EXPECT(grid);

        auto gridspec = grid->spec_str();
        EXPECT(gridspec == R"({"grid":")" + name + R"("})");
    }
#endif
}


CASE("grid: reduced_gg") {
    std::unique_ptr<const Grid> o16(GridFactory::build(spec::Custom({{"grid", "o16"}})));

    EXPECT(o16->spec_str() == R"({"grid":"O16"})");

    std::unique_ptr<const Grid> n16(GridFactory::build(spec::Custom({{"grid", "n16"}})));

    EXPECT(n16->spec_str() == R"({"grid":"N16"})");

    std::unique_ptr<const Grid> known_pl_1(GridFactory::build(
        spec::Custom({{"pl", pl_type{20, 27, 32, 40, 45, 48, 60, 60, 64, 64, 64, 64, 64, 64, 64, 64,
                                     64, 64, 64, 64, 64, 64, 64, 64, 60, 60, 48, 45, 40, 32, 27, 20}}})));

    EXPECT(known_pl_1->spec_str() == R"({"grid":"N16"})");

    std::unique_ptr<const Grid> known_pl_2(
        GridFactory::build(spec::Custom({{"pl", pl_type{20, 24, 28, 32, 32, 28, 24, 20}}})));

    EXPECT(known_pl_2->spec_str() == R"({"grid":"O4"})");

    std::unique_ptr<const Grid> unknown_pl(
        GridFactory::build(spec::Custom({{"pl", pl_type{20, 24, 28, 32, 32, 28, 24, 99}}})));

    EXPECT(unknown_pl->spec_str() == R"({"grid":"N4","pl":[20,24,28,32,32,28,24,99]})");
}


CASE("grid: HEALPix") {
    std::unique_ptr<const Grid> h2(GridFactory::build(spec::Custom({{"grid", "h2"}})));

    EXPECT(h2->spec_str() == R"({"grid":"H2"})");

    std::unique_ptr<const Grid> h2n(GridFactory::build(spec::Custom({{"grid", "H2"}, {"order", "nested"}})));

    EXPECT(h2n->spec_str() == R"({"grid":"H2n"})");
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
