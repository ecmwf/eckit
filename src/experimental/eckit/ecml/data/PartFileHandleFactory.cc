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

#include "eckit/ecml/data/PartFileHandleFactory.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/parser/StringTools.h"

using namespace eckit;
using namespace std;

PartFileHandleFactory::PartFileHandleFactory()
: DataHandleFactory("partfile")
{}

DataHandle* PartFileHandleFactory::makeHandle(const string& descriptor) const
{
    vector<string> ps (StringTools::split(":", descriptor));
    string fileName (ps[0]);
    vector<string> range (StringTools::split(",", ps[ps.size() - 1]));

    Offset offset (atoi(range[0].c_str()));
    Length length (atoi(range[1].c_str()));

    return new PartFileHandle(fileName, offset, length);
}

static PartFileHandleFactory fileHandleFactory;

