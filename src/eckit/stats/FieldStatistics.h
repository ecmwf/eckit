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

#include <iosfwd>
#include <string>


namespace eckit {
class JSON;
class Parametrisation;
}  // namespace eckit


namespace eckit::stats {


class FieldStatistics {
public:
    // -- Constructors

    explicit FieldStatistics(const Parametrisation&);

    FieldStatistics(const FieldStatistics&) = delete;
    FieldStatistics(FieldStatistics&&)      = delete;

    // -- Destructor

    virtual ~FieldStatistics();

    // -- Operators

    void operator=(const FieldStatistics&) = delete;
    void operator=(FieldStatistics&&)      = delete;

    // -- Methods

    virtual double value() const                             = 0;
    virtual void count(const double&)                        = 0;
    virtual void reset(double missingValue, bool hasMissing) = 0;

protected:
    // -- Methods

    virtual void json(eckit::JSON&) const   = 0;
    virtual void print(std::ostream&) const = 0;

    static void json_tv(eckit::JSON&, const std::string& type, double value);

private:
    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const FieldStatistics& r) {
        r.print(out);
        return out;
    }

    friend eckit::JSON& operator<<(eckit::JSON& out, const FieldStatistics& r) {
        r.json(out);
        return out;
    }
};


class FieldStatisticsFactory {
    std::string name_;
    virtual FieldStatistics* make(const Parametrisation&) = 0;

    FieldStatisticsFactory(const FieldStatisticsFactory&)            = delete;
    FieldStatisticsFactory& operator=(const FieldStatisticsFactory&) = delete;

protected:
    FieldStatisticsFactory(const std::string&);
    virtual ~FieldStatisticsFactory();

public:
    static void list(std::ostream&);
    static FieldStatistics* build(const std::string&, const Parametrisation&);
};


template <class T>
class FieldStatisticsBuilder : public FieldStatisticsFactory {
    FieldStatistics* make(const Parametrisation& param) override { return new T(param); }

public:
    FieldStatisticsBuilder(const std::string& name) : FieldStatisticsFactory(name) {}
};


}  // namespace eckit::stats
