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
/// @date JUl 2015

#ifndef eckit_YAMLConfiguration_H
#define eckit_YAMLConfiguration_H

#include "eckit/config/Configuration.h"
#include "eckit/io/SharedBuffer.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

class PathName;
class Stream;

//----------------------------------------------------------------------------------------------------------------------

class YAMLConfiguration : public Configuration, private eckit::NonCopyable {

public:

    YAMLConfiguration(const PathName& path, char separator = '.');
    YAMLConfiguration(std::istream&, char separator = '.');
    YAMLConfiguration(Stream&, char separator = '.');
    YAMLConfiguration(const std::string&, char separator = '.');
    YAMLConfiguration(const SharedBuffer&, char separator = '.');

    ~YAMLConfiguration() override;

private:  // members

    std::string path_;

    void print(std::ostream&) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
