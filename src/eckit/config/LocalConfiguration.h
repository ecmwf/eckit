/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jul 2015


#ifndef eckit_LocalConfiguration_H
#define eckit_LocalConfiguration_H

#include "eckit/config/Configuration.h"


namespace eckit {

class PathName;

class LocalConfiguration : public Configuration {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    LocalConfiguration(char separator = '.');
    LocalConfiguration(const Configuration &other);
    LocalConfiguration(const Configuration &other, const std::string &path);
    LocalConfiguration(const Value& root, char separator = '.');

    virtual ~LocalConfiguration(); // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void set(const std::string &name, const std::string &value);
    void set(const std::string &name, const char *value);
    void set(const std::string &name, double value);
    void set(const std::string &name, long value);
    void set(const std::string &name, bool value);

    // -- Overridden methods


    // -- Class members
    // None

    // -- Class methods


  protected:


    // -- Destructor

    // -- Members
    // None

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

    // LocalConfiguration(const LocalConfiguration &);
    // LocalConfiguration &operator=(const LocalConfiguration &);


    // -- Members


    // -- Methods

    void set(const std::vector<std::string> &path, size_t i, Value &root, const Value &value);
    void set(const std::string &s, const Value &value);

    // -- Overridden methods

    virtual void print(std::ostream &) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const LocalConfiguration& p)
    //  { p.print(s); return s; }

};

} // namespace eckit

#endif

