// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Apr 2015


#ifndef eckit_Configured_H
#define eckit_Configured_H

#include <iosfwd>
#include <string>
#include <vector>


namespace eckit {


class Configured {
public:

    // -- Exceptions
    // None

    // -- Contructors

    Configured();

    // -- Destructor

    virtual ~Configured();  // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual Configured& set(const std::string& name, const std::string& value) = 0;
    virtual Configured& set(const std::string& name, const char* value)        = 0;
    virtual Configured& set(const std::string& name, float value)              = 0;
    virtual Configured& set(const std::string& name, double value)             = 0;
    virtual Configured& set(const std::string& name, int value)                = 0;
    virtual Configured& set(const std::string& name, long value)               = 0;
    virtual Configured& set(const std::string& name, long long value);
    virtual Configured& set(const std::string& name, bool value)   = 0;
    virtual Configured& set(const std::string& name, size_t value) = 0;

    virtual Configured& set(const std::string& name, const std::vector<bool>& value);
    virtual Configured& set(const std::string& name, const std::vector<int>& value)  = 0;
    virtual Configured& set(const std::string& name, const std::vector<long>& value) = 0;
    virtual Configured& set(const std::string& name, const std::vector<long long>& value);
    virtual Configured& set(const std::string& name, const std::vector<size_t>& value)      = 0;
    virtual Configured& set(const std::string& name, const std::vector<float>& value)       = 0;
    virtual Configured& set(const std::string& name, const std::vector<double>& value)      = 0;
    virtual Configured& set(const std::string& name, const std::vector<std::string>& value) = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members

    // -- Methods


    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed


    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
};

}  // namespace eckit
#endif
