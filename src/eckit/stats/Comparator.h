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
/// @author Pedro Maciel
/// @date Apr 2015


#ifndef mir_stats_Comparator_h
#define mir_stats_Comparator_h

#include <iosfwd>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/NonCopyable.h"


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


class Comparator : public eckit::NonCopyable {
public:

    // -- Exceptions
    // None

    // -- Constructors

    Comparator(const param::MIRParametrisation&, const param::MIRParametrisation&);

    // -- Destructor

    virtual ~Comparator();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual std::string execute(const data::MIRField&, const data::MIRField&) = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members

    const param::MIRParametrisation& parametrisation1_;
    const param::MIRParametrisation& parametrisation2_;

    // -- Methods

    virtual void print(std::ostream &) const = 0;   

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

    friend std::ostream& operator<<(std::ostream& out, const Comparator& r) {
        r.print(out);
        return out;
    }

};


class ComparatorFactory {
private:
    std::string name_;
    virtual Comparator* make(const param::MIRParametrisation&, const param::MIRParametrisation&) = 0;
protected:
    ComparatorFactory(const std::string&);
    virtual ~ComparatorFactory();
public:
    static void list(std::ostream&);
    static Comparator* build(const std::string&, const param::MIRParametrisation&, const param::MIRParametrisation&);
};


template<class T>
class ComparatorBuilder : public ComparatorFactory {
    virtual Comparator* make(const param::MIRParametrisation& param1, const param::MIRParametrisation& param2) {
        return new T(param1, param2);
    }
public:
    ComparatorBuilder(const std::string& name) : ComparatorFactory(name) {
    }
};


}  // namespace stats
}  // namespace mir


#endif
