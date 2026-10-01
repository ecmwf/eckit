// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date JUl 2015

#ifndef eckit_YAMLConfiguration_H
#define eckit_YAMLConfiguration_H

#include "eckit/config/Configuration.h"
#include "eckit/io/SharedBuffer.h"

namespace eckit {

class PathName;
class Stream;

//----------------------------------------------------------------------------------------------------------------------

class YAMLConfiguration : public Configuration {

public:

    YAMLConfiguration(const PathName& path, char separator = '.');
    YAMLConfiguration(std::istream&, char separator = '.');
    YAMLConfiguration(Stream&, char separator = '.');
    YAMLConfiguration(const std::string&, char separator = '.');
    YAMLConfiguration(const SharedBuffer&, char separator = '.');

    YAMLConfiguration(const YAMLConfiguration&)            = delete;
    YAMLConfiguration& operator=(const YAMLConfiguration&) = delete;
    YAMLConfiguration(YAMLConfiguration&&)                 = delete;
    YAMLConfiguration& operator=(YAMLConfiguration&&)      = delete;

    ~YAMLConfiguration() override;

private:  // members

    std::string path_;

    void print(std::ostream&) const override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
