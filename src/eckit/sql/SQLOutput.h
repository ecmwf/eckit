// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Simon Smart
/// @date Dec 2003

#ifndef eckit_sql_SQLOutput_H
#define eckit_sql_SQLOutput_H

#include "eckit/sql/SQLOutputConfig.h"

namespace eckit::sql {

namespace expression {
class Expressions;
}

class SQLSelect;

//----------------------------------------------------------------------------------------------------------------------

class SQLOutput {
public:

    SQLOutput();

    SQLOutput(const SQLOutput&)            = delete;
    SQLOutput& operator=(const SQLOutput&) = delete;
    SQLOutput(SQLOutput&&)                 = delete;
    SQLOutput& operator=(SQLOutput&&)      = delete;

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

    // -- Friends
    friend std::ostream& operator<<(std::ostream& s, const SQLOutput& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
