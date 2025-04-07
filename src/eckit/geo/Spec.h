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

#include <ostream>

#include "eckit/config/Parametrisation.h"


namespace eckit {
class JSON;
}


namespace eckit::geo {


class Spec : public Parametrisation {
public:

    Spec()           = default;
    ~Spec() override = default;

    Spec(const Spec&) = delete;
    Spec(Spec&&)      = delete;

    Spec& operator=(const Spec&) = delete;
    Spec& operator=(Spec&&)      = delete;

    bool operator==(const Spec& other) const { return str() == other.str(); }
    bool operator!=(const Spec& other) const { return !operator==(other); }

    std::string get_string(const std::string& name) const;
    bool get_bool(const std::string& name) const;
    int get_int(const std::string& name) const;
    long get_long(const std::string& name) const;
    size_t get_unsigned(const std::string& name) const;
    double get_double(const std::string& name) const;

    std::vector<long> get_long_vector(const std::string& name) const;
    std::vector<size_t> get_unsigned_vector(const std::string& name) const;
    std::vector<double> get_double_vector(const std::string& name) const;

    std::string get_string(const std::string& name, const std::string&) const;
    bool get_bool(const std::string& name, const bool&) const;
    int get_int(const std::string& name, const int&) const;
    long get_long(const std::string& name, const long&) const;
    size_t get_unsigned(const std::string& name, const size_t&) const;
    double get_double(const std::string& name, const double&) const;

    std::vector<long> get_long_vector(const std::string& name, const std::vector<long>&) const;
    std::vector<size_t> get_unsigned_vector(const std::string& name, const std::vector<size_t>&) const;
    std::vector<double> get_double_vector(const std::string& name, const std::vector<double>&) const;

    std::string str() const;

    virtual void json(JSON&) const = 0;

private:

    virtual void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& out, const Spec& spec) {
        spec.print(out);
        return out;
    }
};


}  // namespace eckit::geo
