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
#include <map>
#include <set>
#include <string>
#include <vector>

#include "eckit/value/Value.h"


namespace eckit {

class Stream;
class MetricsCollector;
//----------------------------------------------------------------------------------------------------------------------

class Metrics {
public:  // methods
    static void set(const std::string& name, const Value& value);

    static void set(const std::string& name, const std::vector<std::string>& value);
    static void set(const std::string& name, const std::set<std::string>& value);

    static void set(const std::string& name, const std::map<std::string, unsigned long long>& value);
    static void timestamp(const std::string& name, time_t value);

    static void error(const std::exception&);

    static void send(Stream&);
    static void receive(Stream&);


private:
    Metrics();
    ~Metrics();
};

class CollectMetrics {

    MetricsCollector *collector_;

    void print(std::ostream& s) const;

public:
    CollectMetrics();
    ~CollectMetrics();


    friend std::ostream& operator<<(std::ostream& s, const CollectMetrics& m) {
        m.print(s);
        return s;
    }
};


class MetricsPrefix {
public:
    MetricsPrefix(const std::string& prefix);
    ~MetricsPrefix();
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
