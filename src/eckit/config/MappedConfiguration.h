/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_MappedConfiguration_H
#define eckit_MappedConfiguration_H

#include <map>
#include <variant>

#include "eckit/config/Configuration.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MappedConfiguration final : public Configuration {
public:
    // -- Types

    using value_type = std::variant<std::string,
                                    bool,
                                    int,
                                    long,
                                    long long,
                                    std::size_t,
                                    float,
                                    double,
                                    std::vector<int>,
                                    std::vector<long>,
                                    std::vector<long long>,
                                    std::vector<std::size_t>,
                                    std::vector<float>,
                                    std::vector<double>,
                                    std::vector<std::string>>;

    using container_type = std::map<std::string, value_type>;

    // -- Exceptions
    // None

    // -- Constructors

    explicit MappedConfiguration(const container_type& = {});
    explicit MappedConfiguration(container_type&&);

    MappedConfiguration(const MappedConfiguration&);
    MappedConfiguration(MappedConfiguration&&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    MappedConfiguration& operator=(const MappedConfiguration&);
    MappedConfiguration& operator=(MappedConfiguration&&);

    // -- Methods

    bool empty() const { return map_.empty(); }

    // (consistent with Configured)
    void set(const std::string&, const std::string&);
    void set(const std::string&, bool);
    void set(const std::string&, int);
    void set(const std::string&, long);
    void set(const std::string&, long long);
    void set(const std::string&, std::size_t);
    void set(const std::string&, float);
    void set(const std::string&, double);

    // (consistent with Configured)
    void set(const std::string&, const std::vector<int>&);
    void set(const std::string&, const std::vector<long>&);
    void set(const std::string&, const std::vector<long long>&);
    void set(const std::string&, const std::vector<std::size_t>&);
    void set(const std::string&, const std::vector<float>&);
    void set(const std::string&, const std::vector<double>&);
    void set(const std::string&, const std::vector<std::string>&);

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

    container_type map_;

    // -- Methods
    // None

    // -- Overridden methods

    void print(std::ostream&) const override;
    void json(JSON&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
