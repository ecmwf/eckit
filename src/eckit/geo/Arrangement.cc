// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Arrangement.h"

#include <algorithm>
#include <cctype>
#include <iterator>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo {


namespace {


struct Entry {
    Arrangement arrangement;
    std::string grid;
    std::string letter;
};


const Entry TABLE[]{
    {ARAKAWA_C_T, "ArakawaC", "T"}, {ARAKAWA_C_U, "ArakawaC", "U"}, {ARAKAWA_C_V, "ArakawaC", "V"},
    {FESOM_C, "FESOM", "C"},        {FESOM_N, "FESOM", "N"},        {ICON_C, "ICON", "C"},
    {ICON_V, "ICON", "V"},          {ICON_E, "ICON", "E"},          {ORCA_F, "ORCA", "F"},
    {ORCA_T, "ORCA", "T"},          {ORCA_U, "ORCA", "U"},          {ORCA_V, "ORCA", "V"},
    {ORCA_W, "ORCA", "W"},
};


}  // namespace


Arrangement arrangement_from_string(const std::string& grid, const std::string& letter) {
    std::string upper(letter);
    std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c) { return std::toupper(c); });

    std::string expected;
    for (const auto& entry : TABLE) {
        if (entry.grid == grid) {
            if (entry.letter == upper) {
                return entry.arrangement;
            }
            expected += (expected.empty() ? "" : ", ") + entry.letter;
        }
    }

    ASSERT_MSG(!expected.empty(), "arrangement_from_string: unknown grid '" + grid + "'");

    // (letters as "F, T, U, V or W")
    if (auto last = expected.rfind(", "); last != std::string::npos) {
        expected.replace(last, 2, " or ");
    }

    throw exception::SpecError(grid + ": {arrangement: " + expected + "} expected", Here());
}


const std::string& arrangement_to_string(Arrangement a) {
    auto it =
        std::find_if(std::begin(TABLE), std::end(TABLE), [a](const auto& entry) { return entry.arrangement == a; });
    if (it == std::end(TABLE)) {
        throw exception::SeriousBug("arrangement_to_string: unknown arrangement " + std::to_string(a), Here());
    }

    return it->letter;
}


}  // namespace eckit::geo
