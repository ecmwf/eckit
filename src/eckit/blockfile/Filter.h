// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iosfwd>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "eckit/blockfile/Format.h"

namespace eckit::blockfile {

//----------------------------------------------------------------------------------------------------------------------

/// Selects blocks by label: a block matches when, for every key in the filter, its value
/// equals one of the listed values or lies in one of the listed ranges. Values are written as
/// text and compared according to the key type. An empty filter matches every block.
class Filter {
public:

    Filter() = default;

    /// Parse `key=value[/value...][,key=...]`. On numeric keys `a/to/b` is an inclusive range,
    /// e.g. `class=od,stream=enfo/oper,param=130/167,step=0/to/24`.
    static Filter parse(const std::string& expression);

    void add(const std::string& key, const std::string& value);
    void addRange(const std::string& key, const std::string& from, const std::string& to);

    bool empty() const { return conditions_.empty(); }

    /// Throws eckit::UserError if the filter uses a key the file does not have, a value that does
    /// not parse as the key type, or a range on a string key.
    bool matches(const std::vector<LabelKey>& keys, const BlockInfo& block) const;

    void print(std::ostream& out) const;

private:

    struct Condition {
        std::vector<std::string> values;
        std::vector<std::pair<std::string, std::string>> ranges;
    };

    std::map<std::string, Condition> conditions_;

    friend std::ostream& operator<<(std::ostream& out, const Filter& filter) {
        filter.print(out);
        return out;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
