/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Simon Smart
/// @date Dec 2015


#ifndef eckit_multiplexer_DataSink_H
#define eckit_multiplexer_DataSink_H

#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/config/Configuration.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/io/Length.h"
#include "eckit/io/Buffer.h"

namespace eckit {
namespace multiplexer {

//----------------------------------------------------------------------------------------------------------------------

class DataSink : private eckit::NonCopyable {

public:

    DataSink();

    virtual ~DataSink();

    virtual void open(const std::string& key) = 0;

    virtual void write(const void* buffer, const Length& length) = 0;

    virtual void close() = 0;

protected:

    virtual void print(std::ostream&) const = 0;

private:

    friend std::ostream &operator<<(std::ostream &s, const DataSink &p) {
        p.print(s);
        return s;
    }

};

//----------------------------------------------------------------------------------------------------------------------

class DataSinkFactory {

    std::string name_;
    virtual DataSink* make(const Configuration& config) = 0;

protected:

    DataSinkFactory(const std::string&);
    virtual ~DataSinkFactory();

public:

    static void list(std::ostream &);
    static DataSink* build(const std::string&, const Configuration& config);

};

template< class T>
class DataSinkBuilder : public DataSinkFactory {

    virtual DataSink* make(const Configuration& config) {
        return new T(config);
    }

public:
    DataSinkBuilder(const std::string &name) : DataSinkFactory(name) {}
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace multiplexer
}  // namespace eckit

#endif

