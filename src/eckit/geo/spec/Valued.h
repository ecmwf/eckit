/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/geo/Spec.h"
#include "eckit/value/Content.h"  // for ValueMap
#include "eckit/value/Value.h"


namespace eckit {
class JSON;
}


namespace eckit::geo::spec {


class Valued final : public Spec {
public:
    explicit Valued(const Value& value) :
        map_(value) {}

    explicit Valued(Value&& value) :
        map_(value) {}

    Valued& operator=(const Value& value) {
        map_ = value;
        return *this;
    }
    Valued& operator=(Value&& value) {
        map_ = value;
        return *this;
    }

    bool has(const std::string& name) const override;

    bool get(const std::string& name, std::string& value) const override;
    bool get(const std::string& name, bool& value) const override;
    bool get(const std::string& name, int& value) const override;
    bool get(const std::string& name, long& value) const override;
    bool get(const std::string& name, long long& value) const override;
    bool get(const std::string& name, std::size_t& value) const override;
    bool get(const std::string& name, float& value) const override;
    bool get(const std::string& name, double& value) const override;
    bool get(const std::string& name, std::vector<int>& value) const override;
    bool get(const std::string& name, std::vector<long>& value) const override;
    bool get(const std::string& name, std::vector<long long>& value) const override;
    bool get(const std::string& name, std::vector<std::size_t>& value) const override;
    bool get(const std::string& name, std::vector<float>& value) const override;
    bool get(const std::string& name, std::vector<double>& value) const override;
    bool get(const std::string& name, std::vector<std::string>& value) const override;

private:
    ValueMap map_;

    void json(JSON& j) const override;
};


}  // namespace eckit::geo::spec
