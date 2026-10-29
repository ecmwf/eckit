// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/blockfile/Filter.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit::blockfile {

namespace {

/// Numeric view of an integer or double label.
double numeric(const LabelValue& value) {
    if (const auto* i = std::get_if<std::int64_t>(&value)) {
        return static_cast<double>(*i);
    }
    return std::get<double>(value);
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

Filter Filter::parse(const std::string& expression) {
    Filter filter;

    std::vector<std::string> terms;
    Tokenizer(",")(expression, terms);
    for (const auto& term : terms) {
        const auto equal = term.find('=');
        if (equal == std::string::npos || equal == 0 || equal + 1 == term.size()) {
            throw UserError("Invalid block filter term `" + term + "`, expected key=value[/value...]", Here());
        }
        const std::string key = term.substr(0, equal);
        std::vector<std::string> values;
        Tokenizer("/")(term.substr(equal + 1), values);
        for (std::size_t i = 0; i < values.size(); ++i) {
            if (i + 2 < values.size() && values[i + 1] == "to") {
                filter.addRange(key, values[i], values[i + 2]);
                i += 2;
            }
            else if (values[i] == "to") {
                throw UserError("Invalid range in block filter term `" + term + "`", Here());
            }
            else {
                filter.add(key, values[i]);
            }
        }
    }
    return filter;
}

void Filter::add(const std::string& key, const std::string& value) {
    conditions_[key].values.push_back(value);
}

void Filter::addRange(const std::string& key, const std::string& from, const std::string& to) {
    conditions_[key].ranges.emplace_back(from, to);
}

bool Filter::matches(const std::vector<LabelKey>& keys, const BlockInfo& block) const {
    for (const auto& [key, condition] : conditions_) {
        std::size_t index = 0;
        while (index < keys.size() && keys[index].name != key) {
            ++index;
        }
        if (index == keys.size()) {
            throw UserError("Block filter key `" + key + "` is not a label of this file", Here());
        }

        const LabelType type    = keys[index].type;
        const LabelValue& label = block.labels.at(index);

        bool found = false;
        for (const auto& value : condition.values) {
            if (parseLabelValue(type, value) == label) {
                found = true;
                break;
            }
        }
        for (const auto& [from, to] : condition.ranges) {
            if (found) {
                break;
            }
            if (type == LabelType::String) {
                throw UserError("Block filter range on `" + key + "` needs a numeric key", Here());
            }
            const double x = numeric(label);
            found          = numeric(parseLabelValue(type, from)) <= x && x <= numeric(parseLabelValue(type, to));
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

void Filter::print(std::ostream& out) const {
    const char* separator = "";
    for (const auto& [key, condition] : conditions_) {
        out << separator << key << '=';
        const char* slash = "";
        for (const auto& value : condition.values) {
            out << slash << value;
            slash = "/";
        }
        for (const auto& [from, to] : condition.ranges) {
            out << slash << from << "/to/" << to;
            slash = "/";
        }
        separator = ",";
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::blockfile
