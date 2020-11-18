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
/// @date   Nov 2020

#ifndef eckit_Metrics_h
#define eckit_Metrics_h


#include <ctime>
#include <set>
#include <string>
#include <vector>

#include "eckit/memory/NonCopyable.h"
#include "eckit/value/Value.h"

namespace eckit {

class Stream;

//----------------------------------------------------------------------------------------------------------------------

class Metrics : eckit::NonCopyable {
public:  // methods
    Metrics();
    ~Metrics();

    void set(const std::string& name, const Value& value);
    void set(const std::string& name, const Value& value, const std::string& prefix);

    // void set(const std::string& name, const std::string& value);

    // void set(const std::string& name, bool value);

    // void set(const std::string& name, int value);
    // void set(const std::string& name, unsigned int value);

    // void set(const std::string& name, long value);
    // void set(const std::string& name, unsigned long value);

    // void set(const std::string& name, long long value);
    // void set(const std::string& name, unsigned long long value);

    // void set(const std::string& name, double value);

    void set(const std::string& name, const std::vector<std::string>& value);
    void set(const std::string& name, const std::set<std::string>& value);

    void set(const std::string& name, const std::vector<std::string>& value, const std::string& prefix);
    void set(const std::string& name, const std::set<std::string>& value, const std::string& prefix);


    // void set(const std::string& name, const std::vector<long long>& value);

    void timestamp(const std::string& name, time_t value);


    void error(const std::exception&);

    void send(Stream&) const;
    void receive(Stream&);


    static Metrics& current();


private:  // members

    std::map<std::string, time_t> timestamps_;

    time_t created_;
    Value metrics_;

private:  // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Metrics& m) {
        m.print(s);
        return s;
    }

    friend class ObjectMetric;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
