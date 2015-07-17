/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date JUl 2015


#ifndef eckit_Configuration_H
#define eckit_Configuration_H

#include <string>

#include "eckit/config/Parametrisation.h"
#include "eckit/value/Value.h"


namespace eckit {


class Configuration : public Parametrisation {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    Configuration(const Configuration &other, const std::string &path);

    virtual ~Configuration(); // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual void set(const std::string &name, const std::string &value);
    virtual void set(const std::string &name, const char *value);

    virtual void set(const std::string &name, double value);
    virtual void set(const std::string &name, long value);
    virtual void set(const std::string &name, bool value);

    // -- Overridden methods

    virtual bool has(const std::string &name) const;

    virtual bool get(const std::string &name, std::string &value) const;
    virtual bool get(const std::string &name, bool &value) const;
    virtual bool get(const std::string &name, long &value) const;
    virtual bool get(const std::string &name, double &value) const;

    virtual bool get(const std::string &name, std::vector<long> &value) const;
    virtual bool get(const std::string &name, std::vector<double> &value) const;
    virtual bool get(const std::string &name, size_t &value) const;

    // -- Class members
    // None

    // -- Class methods


  protected:

    Configuration(const Value &root, char separator = '.');

    // -- Destructor

    // -- Members

    Value root_;
    char separator_;

    // -- Methods

    // void print(ostream&) const; // Change to virtual if base class

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

  private:

    // No copy allowed

    Configuration(const Configuration &);
    Configuration &operator=(const Configuration &);

    // -- Members



    // -- Methods

    Value lookUp(const std::string &, bool &) const;
    Value lookUp(const std::string &) const;

    void set(const std::vector<std::string> &path, size_t i, Value &root, const Value &value);
    void set(const std::string &s, const Value &value);

    // -- Overridden methods

    // From MIRParametrisation
    virtual void print(std::ostream &) const;



    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s,const Configuration& p)
     { p.print(s); return s; }

};


}  // namespace eckit
#endif

