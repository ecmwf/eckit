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
/// @date Aug 2018

#ifndef eckit_sql_SQLDistinctOutput_H
#define eckit_sql_SQLDistinctOutput_H


#include "eckit/sql/SQLOutput.h"

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLDistinctOutput : public SQLOutput {

public:  // methods
    SQLDistinctOutput(SQLOutput& output);
    virtual ~SQLDistinctOutput() override;

private:  // methods
    virtual void print(std::ostream&) const override;

    // -- Members

    SQLOutput& output_;
    std::set<std::vector<double>> seen_;
    std::vector<double> tmp_;
    std::vector<size_t> offsets_;

    // -- Overridden methods
    virtual void reset() override;
    virtual void flush() override;
    virtual bool cachedNext() override;
    virtual bool output(const expression::Expressions&) override;
    virtual void preprepare(SQLSelect&) override;
    virtual void prepare(SQLSelect&) override;
    virtual void updateTypes(SQLSelect&) override;
    virtual void cleanup(SQLSelect&) override;
    virtual unsigned long long count() override;

    // Overridden (and removed) functions

    virtual void outputReal(double, bool) override;
    virtual void outputDouble(double, bool) override;
    virtual void outputInt(double, bool) override;
    virtual void outputUnsignedInt(double, bool) override;
    virtual void outputString(const char*, size_t, bool) override;
    virtual void outputBitfield(double, bool) override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit

#endif
