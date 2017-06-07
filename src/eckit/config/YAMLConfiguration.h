/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date JUl 2015


#ifndef eckit_YAMLConfiguration_H
#define eckit_YAMLConfiguration_H

#include "eckit/config/Configuration.h"
#include "eckit/io/Buffer.h"


namespace eckit {

class PathName;
class Stream;

class YAMLConfiguration : public Configuration {
  public:

    // -- Exceptions
    // None

    // -- Contructors

    YAMLConfiguration(const PathName &path, char separator = '.');
    YAMLConfiguration(std::istream &, char separator = '.');
    YAMLConfiguration(Stream&, char separator = '.');
    YAMLConfiguration(const std::string&, char separator = '.');
    YAMLConfiguration(const SharedBuffer&, char separator = '.');

    virtual ~YAMLConfiguration(); // Change to virtual if base class

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods


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

    YAMLConfiguration(const YAMLConfiguration &);
    YAMLConfiguration &operator=(const YAMLConfiguration &);

    // Make private so no one modifies it

    // -- Members

    std::string path_;


    // -- Methods


    // -- Overridden methods

    // From MIRParametrisation
    virtual void print(std::ostream &) const;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    //friend ostream& operator<<(ostream& s,const YAMLConfiguration& p)
    //  { p.print(s); return s; }

};


} // namespace eckit
#endif

