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

#include "eckit/utils/DataHandleFactory.h"

#include "eckit/io/MultiHandle.h"
#include "eckit/parser/StringTools.h"

using namespace std;
using namespace eckit;

typedef StringTools S;

DataHandleFactory::DataHandleFactory(const string& prefix)
: prefix_(prefix)
{
    factories()[prefix] = this;
}

DataHandleFactory::~DataHandleFactory()
{
    factories().erase(prefix_);
}

string DataHandleFactory::prefix() const { return prefix_; }

DataHandleFactory::storage_t &DataHandleFactory::factories()
{
    static storage_t factories;
    return factories;
}

DataHandle* DataHandleFactory::makeHandle(const string& prefix, const string& descriptor)
{
    if (factories().find(prefix) == factories().end())
        throw UserError(string("No factory for '") + prefix + "://' data descriptors");

    return factories()[prefix]->makeHandle(descriptor);
}

pair<string,string> DataHandleFactory::splitPrefix(const string& handleDescriptor)
{
    const string delimiter("://");
    size_t pos (handleDescriptor.find(delimiter));
    if (pos != std::string::npos)
        return make_pair(handleDescriptor.substr(0, pos), handleDescriptor.substr(pos + delimiter.size()));
    
    if (S::startsWith(S::lower(S::trim(handleDescriptor)), "retrieve,")
        || S::startsWith(S::lower(S::trim(handleDescriptor)), "archive,"))
        return make_pair(string("mars"), handleDescriptor); 

    return make_pair(string("file"), handleDescriptor); 
}

void DataHandleFactory::buildMultiHandle(MultiHandle& mh, const std::vector<std::string>& dataDescriptors)
{
    vector<DataHandle*> handles;
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

