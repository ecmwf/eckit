/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, May 2015

#ifndef eckit_utils_DataHandleFactory_H
#define eckit_utils_DataHandleFactory_H

#include <string>
#include <map>

#include "eckit/filesystem/PathName.h"
#include "eckit/ecml/parser/Request.h"

namespace eckit { 

class DataHandle;
class MultiHandle;
class Length; 

class DataHandleFactory {
public:  // Types
    typedef std::map<std::string, DataHandleFactory*> Storage;

public:
    virtual ~DataHandleFactory();

    static DataHandle* openForRead(const std::string&);
    static DataHandle* openForWrite(const std::string&, const Length& = Length(0));

    static void buildMultiHandle(MultiHandle&, const std::vector<std::string>&);
    static void buildMultiHandle(MultiHandle&, const std::string&);

protected:
    DataHandleFactory(const std::string&);

    static std::pair<std::string,std::string> splitPrefix(const std::string&);

    static DataHandle* makeHandle(const std::string&, const std::string&);

    virtual DataHandle* makeHandle(const std::string&) const = 0;
    std::string prefix() const;

private:
    DataHandleFactory();
    static Storage& factories();

    std::string prefix_;
};

} // namespace eckit

#endif
