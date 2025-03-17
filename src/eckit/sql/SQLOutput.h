/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Simon Smart
/// @date Dec 2003

#ifndef eckit_sql_SQLOutput_H
#define eckit_sql_SQLOutput_H

#include "eckit/memory/NonCopyable.h"
#include "eckit/sql/SQLOutputConfig.h"

namespace eckit::sql {

namespace expression {
class Expressions;
}

class SQLSelect;

//----------------------------------------------------------------------------------------------------------------------

class SQLOutput : private eckit::NonCopyable {
public:

    SQLOutput();
    virtual ~SQLOutput();

    virtual void preprepare(SQLSelect&);
    virtual void prepare(SQLSelect&) = 0;
    virtual void updateTypes(SQLSelect&) {}
    virtual void cleanup(SQLSelect&) = 0;

    virtual void reset() = 0;
    virtual void flush() = 0;

    /// If an iterator in the chain has started caching output (e.g. the
    /// OrderBy iterator), start flushing those through. Returns true
    /// when row is output, false otherwise.
    virtual bool cachedNext();

    virtual bool output(const expression::Expressions&) = 0;

    virtual void outputReal(double, bool)                = 0;
    virtual void outputDouble(double, bool)              = 0;
    virtual void outputInt(double, bool)                 = 0;
    virtual void outputUnsignedInt(double, bool)         = 0;
    virtual void outputString(const char*, size_t, bool) = 0;
    virtual void outputBitfield(double, bool)            = 0;

    virtual unsigned long long count() = 0;

protected:

    virtual void print(std::ostream&) const;

private:

    // No copy allowed
    SQLOutput(const SQLOutput&);
    SQLOutput& operator=(const SQLOutput&);
    // -- Friends
    friend std::ostream& operator<<(std::ostream& s, const SQLOutput& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
