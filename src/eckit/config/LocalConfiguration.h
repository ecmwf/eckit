/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   July 2015

#ifndef eckit_LocalConfiguration_H
#define eckit_LocalConfiguration_H

#include <vector>

#include "eckit/config/Configuration.h"
#include "eckit/config/Configured.h"
#include "eckit/types/Types.h"


namespace eckit {

class PathName;
class Stream;

//----------------------------------------------------------------------------------------------------------------------

class LocalConfiguration : public Configuration, public Configured {
    /// @note Do NOT expose eckit::Value in the interface of configuration
    ///       eckit::Value should remain an internal detail of configuration objects

public:  // methods
    explicit LocalConfiguration(char separator = '.');
    explicit LocalConfiguration(Stream& s);
    explicit LocalConfiguration(const Configuration& other);

    LocalConfiguration(const Configuration& other, const std::string& path);

    virtual ~LocalConfiguration() override;

    LocalConfiguration& set(const std::string& name, const std::string& value) override;
    LocalConfiguration& set(const std::string& name, const char* value) override;
    LocalConfiguration& set(const std::string& name, bool value) override;
    LocalConfiguration& set(const std::string& name, int value) override;
    LocalConfiguration& set(const std::string& name, long value) override;
    LocalConfiguration& set(const std::string& name, long long value);
    LocalConfiguration& set(const std::string& name, size_t value) override;
    LocalConfiguration& set(const std::string& name, float value) override;
    LocalConfiguration& set(const std::string& name, double value) override;

    LocalConfiguration& set(const std::string& name, const std::vector<int>& value) override;
    LocalConfiguration& set(const std::string& name, const std::vector<long>& value) override;
    LocalConfiguration& set(const std::string& name, const std::vector<long long>& value);
    LocalConfiguration& set(const std::string& name, const std::vector<size_t>& value) override;
    LocalConfiguration& set(const std::string& name, const std::vector<float>& value) override;
    LocalConfiguration& set(const std::string& name, const std::vector<double>& value) override;
    LocalConfiguration& set(const std::string& name, const std::vector<std::string>& value) override;

    LocalConfiguration& set(const std::string& name, const LocalConfiguration& value);
    LocalConfiguration& set(const std::string& name, const std::vector<LocalConfiguration>& value);

protected:
    friend class Configuration;

    /// to be used only by class Configuration
    LocalConfiguration(const eckit::Value&, char separator = '.');

    virtual void print(std::ostream&) const override;

private:
    void setValue(const std::vector<std::string>& path, size_t i, Value& root, const Value& value);
    void setValue(const std::string& s, const Value& value);
};

//----------------------------------------------------------------------------------------------------------------------

template <>
struct VectorPrintSelector<LocalConfiguration> {
    typedef VectorPrintSimple selector;
};

}  // namespace eckit

#endif
