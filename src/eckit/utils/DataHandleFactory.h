/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, May 2015

#ifndef DataHandleFactory_H
#define DataHandleFactory_H

#include <string>
#include <map>

#include "eckit/filesystem/PathName.h"
#include "eckit/parser/Request.h"

namespace eckit { 

class DataHandle;
class MultiHandle;
class Length; 

class DataHandleFactory {
public:
    virtual ~DataHandleFactory();

    static eckit::DataHandle* openForRead(const std::string&);
    static eckit::DataHandle* openForWrite(const std::string&, const eckit::Length& = eckit::Length(0));

    static void buildMultiHandle(eckit::MultiHandle&, const std::vector<std::string>&);

protected:
    DataHandleFactory(const std::string&);

    static std::pair<std::string,std::string> splitPrefix(const std::string&);

    static eckit::DataHandle* makeHandle(const std::string&, const std::string&);

    virtual eckit::DataHandle* makeHandle(const std::string&) const = 0;
    std::string prefix() const;

private:
    DataHandleFactory();

    std::string prefix_;

    static std::map<std::string, DataHandleFactory*> factories_;
};

} // namespace eckit

#endif
