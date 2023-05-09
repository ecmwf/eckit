/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <string>
#include <vector>


namespace eckit::geo {


class Parametrisation {
public:
    // -- Types

    using key_type = std::string;

    // -- Exceptions
    // None

    // -- Constructors

    Parametrisation()                       = default;
    Parametrisation(const Parametrisation&) = delete;
    Parametrisation(Parametrisation&&)      = delete;

    // -- Destructor

    virtual ~Parametrisation() = default;

    // -- Convertors
    // None

    // -- Operators

    Parametrisation& operator=(const Parametrisation&) = delete;
    Parametrisation& operator=(Parametrisation&&)      = delete;

    // -- Methods

    virtual bool has(const key_type&) const = 0;

    virtual bool get(const key_type&, bool&) const          = 0;
    virtual bool get(const key_type&, int&) const           = 0;
    virtual bool get(const key_type&, unsigned int&) const  = 0;
    virtual bool get(const key_type&, long&) const          = 0;
    virtual bool get(const key_type&, unsigned long&) const = 0;
    virtual bool get(const key_type&, float&) const         = 0;
    virtual bool get(const key_type&, double&) const        = 0;
    virtual bool get(const key_type&, std::string&) const   = 0;

    virtual bool get(const key_type&, std::vector<bool>&) const          = 0;
    virtual bool get(const key_type&, std::vector<int>&) const           = 0;
    virtual bool get(const key_type&, std::vector<unsigned int>&) const  = 0;
    virtual bool get(const key_type&, std::vector<long>&) const          = 0;
    virtual bool get(const key_type&, std::vector<unsigned long>&) const = 0;
    virtual bool get(const key_type&, std::vector<float>&) const         = 0;
    virtual bool get(const key_type&, std::vector<double>&) const        = 0;
    virtual bool get(const key_type&, std::vector<std::string>&) const   = 0;

    bool get_bool(const key_type&) const;
    int get_int(const key_type&) const;
    unsigned int get_unsigned_int(const key_type&) const;
    long get_long(const key_type&) const;
    unsigned long get_unsigned_long(const key_type&) const;
    float get_float(const key_type&) const;
    double get_double(const key_type&) const;
    std::string get_string(const key_type&) const;

    std::vector<bool> get_vector_bool(const key_type&) const;
    std::vector<int> get_vector_int(const key_type&) const;
    std::vector<unsigned int> get_vector_unsigned_int(const key_type&) const;
    std::vector<long> get_vector_long(const key_type&) const;
    std::vector<unsigned long> get_vector_unsigned_long(const key_type&) const;
    std::vector<float> get_vector_float(const key_type&) const;
    std::vector<double> get_vector_double(const key_type&) const;
    std::vector<std::string> get_vector_string(const key_type&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
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

private:
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
    // None
};


}  // namespace eckit::geo
