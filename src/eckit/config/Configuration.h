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


#ifndef eckit_Configuration_H
#define eckit_Configuration_H

#include <cstdint>
#include <memory>
#include <type_traits>
#include "eckit/config/Parametrisation.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LocalConfiguration;
class JSON;
class Value;
class Hash;

class Configuration : public Parametrisation {
    /// @note Do NOT expose eckit::Value in the interface of configuration
    ///       eckit::Value should remain an internal detail of configuration objects
    ///       Clients should use typed configuration parameters

public:  // methods

    // -- Destructor

    ~Configuration() override;

    // Fast access, will throw an exception

    bool getBool(const std::string& name) const;
    int getInt(const std::string& name) const;
    long getLong(const std::string& name) const;
    std::size_t getUnsigned(const std::string& name) const;
    std::int32_t getInt32(const std::string& name) const;
    std::int64_t getInt64(const std::string& name) const;
    float getFloat(const std::string& name) const;
    double getDouble(const std::string& name) const;
    std::string getString(const std::string& name) const;

    std::vector<int> getIntVector(const std::string& name) const;
    std::vector<long> getLongVector(const std::string& name) const;
    std::vector<std::size_t> getUnsignedVector(const std::string& name) const;
    std::vector<std::int32_t> getInt32Vector(const std::string& name) const;
    std::vector<std::int64_t> getInt64Vector(const std::string& name) const;
    std::vector<float> getFloatVector(const std::string& name) const;
    std::vector<double> getDoubleVector(const std::string& name) const;
    std::vector<std::string> getStringVector(const std::string& name) const;

    // Access with default in case of falure

    bool getBool(const std::string& name, const bool& defaultValue) const;
    int getInt(const std::string& name, const int& defaultValue) const;
    long getLong(const std::string& name, const long& defaultValue) const;
    std::size_t getUnsigned(const std::string& name, const std::size_t& defaultValue) const;
    std::int32_t getInt32(const std::string& name, const std::int32_t& defaultValue) const;
    std::int64_t getInt64(const std::string& name, const std::int64_t& defaultValue) const;
    float getFloat(const std::string& name, const float& defaultValue) const;
    double getDouble(const std::string& name, const double& defaultValue) const;
    std::string getString(const std::string& name, const std::string& defaultValue) const;

    std::vector<int> getIntVector(const std::string& name, const std::vector<int>& defaultValue) const;
    std::vector<long> getLongVector(const std::string& name, const std::vector<long>& defaultValue) const;
    std::vector<std::size_t> getUnsignedVector(const std::string& name,
                                               const std::vector<std::size_t>& defaultValue) const;
    std::vector<std::int32_t> getInt32Vector(const std::string& name,
                                             const std::vector<std::int32_t>& defaultValue) const;
    std::vector<std::int64_t> getInt64Vector(const std::string& name,
                                             const std::vector<std::int64_t>& defaultValue) const;
    std::vector<float> getFloatVector(const std::string& name, const std::vector<float>& defaultValue) const;
    std::vector<double> getDoubleVector(const std::string& name, const std::vector<double>& defaultValue) const;
    std::vector<std::string> getStringVector(const std::string& name,
                                             const std::vector<std::string>& defaultValue) const;

    bool empty() const;

    std::vector<std::string> keys() const;

    // Access to LocalConfiguration

    std::vector<LocalConfiguration> getSubConfigurations(const std::string& name) const;

    std::vector<LocalConfiguration> getSubConfigurations() const;

    LocalConfiguration getSubConfiguration(const std::string& name) const;

    char separator() const;

    // -- Overridden methods

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

    bool get(const std::string& name, std::vector<LocalConfiguration>&) const;
    bool get(const std::string& name, LocalConfiguration&) const;

    /// @todo This method should be protected. As per note above,
    ///       we don't want to expose eckit::Value out of Configuration.
    [[deprecated(
        "eckit::Value should not be exposed via eckit::Configuration::get(). This method Will be removed in a next "
        "release.")]]
    const Value& get() const {
        return getValue();
    }

    virtual void hash(eckit::Hash&) const;

    // -- Introspection methods

    bool isSubConfiguration(const std::string& name) const;

    bool isIntegral(const std::string& name) const;

    bool isBoolean(const std::string& name) const;

    bool isFloatingPoint(const std::string& name) const;

    bool isString(const std::string& name) const;

    bool isList(const std::string& name) const;

    bool isSubConfigurationList(const std::string& name) const;

    bool isIntegralList(const std::string& name) const;

    bool isBooleanList(const std::string& name) const;

    bool isFloatingPointList(const std::string& name) const;

    bool isStringList(const std::string& name) const;

    bool isNull(const std::string& name) const;

    template <typename T>
    bool isConvertible(const std::string& name) const {
        using _T = std::decay_t<T>;
        if constexpr (std::is_base_of_v<LocalConfiguration, _T>) {
            return isSubConfiguration(name);
        }
        else if constexpr (std::is_same_v<_T, int> || std::is_same_v<_T, long> || std::is_same_v<_T, long long> ||
                           std::is_same_v<_T, std::size_t>) {
            return isIntegral(name) || isBoolean(name);
        }
        else if constexpr (std::is_same_v<_T, float> || std::is_same_v<_T, double>) {
            return isFloatingPoint(name) || isIntegral(name) || isBoolean(name);
        }
        else if constexpr (std::is_same_v<_T, std::string>) {
            return isString(name);
        }
        else if constexpr (is_vector<_T>::value) {
            using _V = std::decay_t<typename _T::value_type>;
            if constexpr (std::is_base_of_v<LocalConfiguration, _V>) {
                return isSubConfigurationList(name);
            }
            else if constexpr (std::is_same_v<_V, int> || std::is_same_v<_V, long> || std::is_same_v<_V, long long> ||
                               std::is_same_v<_V, std::size_t>) {
                return isIntegralList(name) || isBooleanList(name);
            }
            else if constexpr (std::is_same_v<_V, float> || std::is_same_v<_V, double>) {
                return isFloatingPointList(name) || isIntegralList(name) || isBooleanList(name);
            }
            else if constexpr (std::is_same_v<_V, std::string>) {
                return isStringList(name);
            }
        }
        return false;
    }

    template <typename T>
    bool isConvertible(const std::string& name, T&) const {
        return isConvertible<T>(name);
    }

protected:  // methods

    Configuration(const eckit::Value&, char separator = '.');

    Configuration(const Configuration&);
    Configuration(const Configuration&,
                  const std::string& path);  ///< sub-select a subconfiguration

    Configuration& operator=(const Configuration&);

    Value lookUp(const std::string&) const;
    Value lookUp(const std::string&, bool&) const;

    operator Value() const;

    const Value& getValue() const;

protected:  // members

    friend class LocalConfiguration;
    std::unique_ptr<Value> root_;
    char separator_;

private:  // methods

    void json(JSON& s) const;
    friend JSON& operator<<(JSON& s, const Configuration& v) {
        v.json(s);
        return s;
    }

    template <class T>
    void _get(const std::string&, T&) const;

    template <class T>
    void _getWithDefault(const std::string& name, T& value, const T& defaultVal) const;

    virtual void print(std::ostream&) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const Configuration& p) {
        p.print(s);
        return s;
    }

private:

    // Helper structs for introspection of template T in isConvertible<T> method
    template <class T>
    struct is_vector {
        using type                  = T;
        constexpr static bool value = false;
    };

    template <class T>
    struct is_vector<std::vector<T>> {
        using type                  = std::vector<T>;
        constexpr static bool value = true;
    };
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
