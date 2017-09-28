/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/filesystem/URI.h"
#include "eckit/parser/Tokenizer.h"
#include "eckit/filesystem/URIManager.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

URI::URI(const std::string& path)
{
    Tokenizer parse(":");
    std::vector<std::string> s;

    parse(path,s);

    switch (s.size()) {
    case 1:
        scheme_ = "unix";
        name_   = s[0];
        break;

    case 2:
        name_   = s[1];
        scheme_ = s[0];
        break;

    default:
        scheme_ = s[0];
        name_   = s[1];
        for (size_t j = 2; j < s.size() ; j++)
            name_ = name_ + ':' + s[j];
        break;
    }
}

URI::~URI()
{
}

bool URI::exists() const
{
    return URIManager::lookUp(scheme_).exists(*this);
}

DataHandle*  URI::newWriteHandle() const
{
    return URIManager::lookUp(scheme_).newWriteHandle(*this);
}

DataHandle*  URI::newReadHandle(const OffsetList& ol, const LengthList& ll) const
{
    return URIManager::lookUp(scheme_).newReadHandle(*this, ol, ll);
}

DataHandle*  URI::newReadHandle() const
{
    return URIManager::lookUp(scheme_).newReadHandle(*this);
}

void URI::print(std::ostream& s) const 
{
    s << "URI[scheme=" << scheme_ << ",name=" << name_ << "]";
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
