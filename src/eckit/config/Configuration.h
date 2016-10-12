/*
 * (C) Copyright 1996-2016 ECMWF.
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


#ifndef eckit_Configuration_H
#define eckit_Configuration_H


#include "eckit/config/Parametrisation.h"
#include "eckit/value/Value.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LocalConfiguration;

class Configuration : public Parametrisation {

    /// @note Do NOT expose eckit::Value in the interface of configuration
    ///       eckit::Value should remain an internal detail of configuration objects
    ///       Clients should use typed configuration parameters

public: // methods

    // Fast access, will throw an exception

    bool getBool(const std::string &name) const;
    int getInt(const std::string &name) const;
    long getLong(const std::string &name) const;
    size_t getUnsigned(const std::string &name) const;
    float getFloat(const std::string &name) const;
    double getDouble(const std::string &name) const;
    std::string getString(const std::string &name) const;

    // Access with default in case of falure

    bool getBool(const std::string &name, const bool& defaultValue) const;
    int getInt(const std::string &name, const int& defaultValue) const;
    long getLong(const std::string &name, const long& defaultValue) const;
    size_t getUnsigned(const std::string &name, const size_t& defaultValue) const;
    float getFloat(const std::string &name, const float& defaultValue) const;
    double getDouble(const std::string &name, const double& defaultValue) const;
    std::string getString(const std::string &name, const std::string& defaultValue) const;


    std::vector<int> getIntVector(const std::string &name) const;
    std::vector<long> getLongVector(const std::string &name) const;
    std::vector<size_t> getUnsignedVector(const std::string &name) const;
    std::vector<float> getFloatVector(const std::string &name) const;
    std::vector<double> getDoubleVector(const std::string &name) const;
    std::vector<std::string> getStringVector(const std::string &name) const;

    // Access to LocalConfiguration

    std::vector<LocalConfiguration> getSubConfigurations(const std::string &name) const;

    LocalConfiguration getSubConfiguration(const std::string &name) const;

    char separator() const;

    // -- Overridden methods

    virtual bool has(const std::string &name) const;

    virtual bool get(const std::string &name, std::string &value) const;
    virtual bool get(const std::string &name, bool &value) const;
    virtual bool get(const std::string &name, long &value) const;
    virtual bool get(const std::string &name, double &value) const;

    virtual bool get(const std::string &name, std::vector<long> &value) const;
    virtual bool get(const std::string &name, std::vector<double> &value) const;
    virtual bool get(const std::string &name, std::vector<std::string> &value) const;
    virtual bool get(const std::string &name, size_t &value) const;

    bool get(const std::string &name, std::vector<LocalConfiguration>&) const;
    bool get(const std::string &name, LocalConfiguration&) const;

    /// @todo This method should be protected. As per note above,
    ///       we don't wnat to expose eckit::Value out of Configuration.
    const Value& get() const { return root_; }

protected: // methods

    Configuration(const eckit::Value&, char separator = '.');

    Configuration(const Configuration&);
    Configuration(const Configuration&, const std::string& path); ///< sub-select a subconfiguration

    Configuration &operator=(const Configuration &);

    virtual ~Configuration();

    Value lookUp(const std::string&) const;
    Value lookUp(const std::string &, bool &) const;

    operator Value() const { return root_; }

protected: // members

    Value root_;
    char separator_;

private: // methods

    template <class T>
    void _get(const std::string&, T&) const;

    template <class T>
    void _getWithDefault(const std::string &name, T& value, const T& defaultVal) const;

    virtual void print(std::ostream &) const = 0;

    friend std::ostream& operator<<(std::ostream& s,const Configuration& p) { p.print(s); return s; }

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

