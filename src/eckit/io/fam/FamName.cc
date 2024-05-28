/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamName.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

constexpr auto sessionName = "EckitFamNameSession";

//----------------------------------------------------------------------------------------------------------------------

FamName::FamName(const net::Endpoint& endpoint, std::string name):
    config_ {FamConfig {endpoint, sessionName}}, name_ {std::move(name)} { }

FamName::FamName(const URI& uri): FamName(uri, uri.name()) {
    ASSERT(uri.scheme() == "fam");
}

FamName::~FamName() = default;

//----------------------------------------------------------------------------------------------------------------------

auto FamName::uri() const -> URI {
    return URI("fam://" + asString());
}

auto FamName::asString() const -> std::string {
    return std::string(config_.endpoint);
}

//----------------------------------------------------------------------------------------------------------------------

auto FamName::config() const -> const FamConfig& {
    return config_;
}

auto FamName::parseName() const -> std::vector<std::string> {
    return Tokenizer("/").tokenize(name_);
}

auto FamName::session() const -> Session {
    return FamSession::instance().getOrAdd(config_);
}

void FamName::print(std::ostream& out) const {
    out << config_ << ",name=" << name_;
}

std::ostream& operator<<(std::ostream& out, const FamName& name) {
    name.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
