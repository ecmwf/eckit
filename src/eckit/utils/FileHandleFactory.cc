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

#include "eckit/utils/FileHandleFactory.h"
#include "eckit/io/FileHandle.h"

using namespace eckit;
using namespace std;

FileHandleFactory::FileHandleFactory()
: DataHandleFactory("file")
{}

DataHandle* FileHandleFactory::makeHandle(const string& fileName) const
{
    return new FileHandle(fileName);
}

static FileHandleFactory fileHandleFactory;

