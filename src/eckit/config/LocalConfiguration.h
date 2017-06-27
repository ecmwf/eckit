/*
 * (C) Copyright 1996-2017 ECMWF.
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

namespace eckit {

class PathName;

//----------------------------------------------------------------------------------------------------------------------

class LocalConfiguration : public Configuration, public Configured {

    /// @note Do NOT expose eckit::Value in the interface of configuration
    ///       eckit::Value should remain an internal detail of configuration objects

public: // methods

    LocalConfiguration(char separator = '.');
    LocalConfiguration(const Configuration &other);
    LocalConfiguration(const Configuration &other, const std::string &path);

    virtual ~LocalConfiguration();

    LocalConfiguration& set(const std::string &name, const std::string &value);
    LocalConfiguration& set(const std::string &name, const char *value);
    LocalConfiguration& set(const std::string &name, bool value);
    LocalConfiguration& set(const std::string &name, int value);
    LocalConfiguration& set(const std::string &name, long value);
    LocalConfiguration& set(const std::string& name, size_t value);
    LocalConfiguration& set(const std::string &name, float value);
    LocalConfiguration& set(const std::string &name, double value);

    LocalConfiguration& set(const std::string& name, const std::vector<int>& value);
    LocalConfiguration& set(const std::string& name, const std::vector<long>& value);
    LocalConfiguration& set(const std::string& name, const std::vector<size_t>& value);
    LocalConfiguration& set(const std::string& name, const std::vector<float>& value);
    LocalConfiguration& set(const std::string& name, const std::vector<double>& value);
    LocalConfiguration& set(const std::string& name, const std::vector<std::string>& value);

    LocalConfiguration& set(const std::string& name, const LocalConfiguration& value);
    LocalConfiguration& set(const std::string& name, const std::vector<LocalConfiguration>& value);

protected:

    friend class Configuration;

    /// to be used only by class Configuration
    LocalConfiguration(const eckit::Value&, char separator = '.');

    virtual void print(std::ostream &) const;

private:

    void setValue(const std::vector<std::string> &path, size_t i, Value &root, const Value &value);
    void setValue(const std::string &s, const Value &value);

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

