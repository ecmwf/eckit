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


#ifndef eckit_multiplexer_DummySink_H
#define eckit_multiplexer_DummySink_H

#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/io/Length.h"
#include "eckit/memory/NonCopyable.h"
#include "sandbox/multiplexer/DataSink.h"

namespace eckit {
namespace multiplexer {

//----------------------------------------------------------------------------------------------------------------------

class DummySink : public DataSink {

public:

    DummySink();

    virtual ~DummySink();

    virtual void open(const std::string& key);

    virtual void write(void* buffer, const Length& length);

    virtual void close();

protected:

    virtual void print(std::ostream&) const;

private:

    std::string key_;

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace multiplexer
}  // namespace eckit

#endif // eckit_multiplexer_DummySink_H

