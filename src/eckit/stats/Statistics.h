/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_stats_Statistics_h
#define mir_stats_Statistics_h

#include <cstddef>
#include <iosfwd>
#include <limits>
#include <string>


namespace mir {
namespace data {
class MIRField;
}
namespace param {
class MIRParametrisation;
}
}


namespace mir {
namespace stats {


class Statistics {
public:

    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Statistics(const param::MIRParametrisation&);

    // -- Destructor

    virtual ~Statistics();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual void execute(const data::MIRField&) = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Types

    /// Counter accounting for missing values, for a single MIRFIeld
    class CounterUnary {
    private:

        size_t count_;
        size_t missing_;
        double missingValue_;
        bool hasMissing_;

    public:

        CounterUnary(const data::MIRField&);
        bool missingValue(const double&);

        size_t count() const { return count_; }
        size_t missing() const { return missing_; }
    };


    /// Counter accounting for missing values, for a MIRFIeld pair
    struct CounterBinary {
    private:

        CounterUnary counter1_;
        CounterUnary counter2_;
        size_t missing1_;
        size_t missing2_;

    public:

        CounterBinary(const data::MIRField&, const data::MIRField&);
        bool missingValues(const double&, const double&);

        size_t missing1() const { return missing1_; }
        size_t missing2() const { return missing2_; }

        size_t count() const;

    };

    struct CountOutside {
    private:

        const double lowerLimit_;
        const double upperLimit_;
        bool hasLowerLimit_;
        bool hasUpperLimit_;
        size_t count_{0};

    public:

        CountOutside(const double& upperLimit=std::numeric_limits<double>::quiet_NaN(),
                     const double& lowerLimit=std::numeric_limits<double>::quiet_NaN());

        size_t count() const;

        void count(const double& v);
    };

    // -- Members

    const param::MIRParametrisation& parametrisation_;

    // -- Methods

    /// Output
    virtual void print(std::ostream&) const = 0;

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

    friend std::ostream& operator<<(std::ostream& out, const Statistics& r) {
        r.print(out);
        return out;
    }

};


class StatisticsFactory {
private:
    std::string name_;
    virtual Statistics* make(const param::MIRParametrisation&) = 0;
protected:
    StatisticsFactory(const std::string&);
    virtual ~StatisticsFactory();
public:
    static void list(std::ostream&);
    static Statistics* build(const std::string&, const param::MIRParametrisation&);
};


template<class T>
class StatisticsBuilder : public StatisticsFactory {
private:
    Statistics* make(const param::MIRParametrisation& param) {
        return new T(param);
    }
public:
    StatisticsBuilder(const std::string& name) : StatisticsFactory(name) {
    }
};


}  // namespace stats
}  // namespace mir


#endif
