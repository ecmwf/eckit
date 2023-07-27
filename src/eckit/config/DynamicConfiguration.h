/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_DynamicConfiguration_H
#define eckit_DynamicConfiguration_H

#include <unordered_set>

#include "eckit/config/Configuration.h"
#include "eckit/config/MappedConfiguration.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class DynamicConfiguration : public Configuration {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    DynamicConfiguration();
    explicit DynamicConfiguration(const Configuration&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void set(const std::string&, std::string&);
    void set(const std::string&, bool&);
    void set(const std::string&, int&);
    void set(const std::string&, long&);
    void set(const std::string&, long long&);
    void set(const std::string&, std::size_t&);
    void set(const std::string&, float&);
    void set(const std::string&, double&);

    void set(const std::string&, std::vector<int>&);
    void set(const std::string&, std::vector<long>&);
    void set(const std::string&, std::vector<long long>&);
    void set(const std::string&, std::vector<std::size_t>&);
    void set(const std::string&, std::vector<float>&);
    void set(const std::string&, std::vector<double>&);
    void set(const std::string&, std::vector<std::string>&);

    void hide(const std::string&);
    void unhide(const std::string&);

    // -- Overridden methods

    bool has(const std::string&) const override;

    bool get(const std::string&, std::string&) const override;
    bool get(const std::string&, bool&) const override;
    bool get(const std::string&, int&) const override;
    bool get(const std::string&, long&) const override;
    bool get(const std::string&, long long&) const override;
    bool get(const std::string&, std::size_t&) const override;
    bool get(const std::string&, float&) const override;
    bool get(const std::string&, double&) const override;

    bool get(const std::string&, std::vector<int>&) const override;
    bool get(const std::string&, std::vector<long>&) const override;
    bool get(const std::string&, std::vector<long long>&) const override;
    bool get(const std::string&, std::vector<std::size_t>&) const override;
    bool get(const std::string&, std::vector<float>&) const override;
    bool get(const std::string&, std::vector<double>&) const override;
    bool get(const std::string&, std::vector<std::string>&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const Configuration& passive_;
    MappedConfiguration active_;

    struct : std::unordered_set<std::string> {
        bool contains(const value_type& key) const { return find(key) != end(); }
    } hide_;

    // -- Methods
    // None

    // -- Overridden methods

    void print(std::ostream&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    template <typename T>
    friend void __set(DynamicConfiguration& config, const std::string& name, T& value) {
        config.active_.set(name, value);
        config.hide_.erase(name);
    }

    template <typename T>
    friend bool __get(const DynamicConfiguration& config, const std::string& name, T& value) {
        return !config.hide_.contains(name) && (config.active_.get(name, value) || config.passive_.get(name, value));
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
