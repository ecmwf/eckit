/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "experimental/eckit/ecml/data/DataHandleFactory.h"

#include "eckit/io/MultiHandle.h"
#include "eckit/parser/StringTools.h"

using namespace eckit;

DataHandleFactory::DataHandleFactory(const std::string& prefix)
: prefix_(prefix)
{
    factories()[prefix] = this;
}

DataHandleFactory::~DataHandleFactory()
{
    factories().erase(prefix_);
}

std::string DataHandleFactory::prefix() const { return prefix_; }

DataHandleFactory::Storage &DataHandleFactory::factories()
{
    static Storage factories;
    return factories;
}

DataHandle* DataHandleFactory::makeHandle(const std::string& prefix, const std::string& descriptor)
{
    if (factories().find(prefix) == factories().end())
        throw UserError(std::string("No factory for '") + prefix + "://' data descriptors");

    return factories()[prefix]->makeHandle(descriptor);
}

std::pair<std::string, std::string> DataHandleFactory::splitPrefix(const std::string& handleDescriptor)
{
    const std::string delimiter("://");
    size_t pos (handleDescriptor.find(delimiter));
    if (pos != std::string::npos)
        return make_pair(handleDescriptor.substr(0, pos), handleDescriptor.substr(pos + delimiter.size()));
    
    if (StringTools::startsWith(StringTools::lower(StringTools::trim(handleDescriptor)), "retrieve,")
        || StringTools::startsWith(StringTools::lower(StringTools::trim(handleDescriptor)), "archive,"))
        return std::make_pair(std::string("mars"), handleDescriptor);

    return std::make_pair(std::string("file"), handleDescriptor);
}

void DataHandleFactory::buildMultiHandle(MultiHandle& mh, const std::vector<std::string>& dataDescriptors)
{
    std::vector<DataHandle*> handles;
    for (size_t i(0); i < dataDescriptors.size(); ++i)
    {
        std::pair<std::string,std::string> p (splitPrefix(dataDescriptors[i]));
        mh += makeHandle(p.first, p.second);
    }
}

DataHandle* DataHandleFactory::openForRead(const std::string& s)
{
    std::pair<std::string,std::string> p (splitPrefix(s));
    DataHandle* d (makeHandle(p.first, p.second));
    d->openForRead();
    return d;
}

DataHandle* DataHandleFactory::openForWrite(const std::string& s, const eckit::Length& length)
{
    std::pair<std::string,std::string> p (splitPrefix(s));
    DataHandle* d (makeHandle(p.first, p.second));
    d->openForWrite(length);
    return d;
}

