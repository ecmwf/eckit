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

#ifndef eckit_sql_SQLSimpleOutput_H
#define eckit_sql_SQLSimpleOutput_H

#include "eckit/sql/SQLOutput.h"

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLSimpleOutput : public SQLOutput {

public:  // methods
    SQLSimpleOutput(const SQLOutputConfig& config, std::ostream& out);
    virtual ~SQLSimpleOutput() override;

private:  // methods
    template <typename T>
    void outputValue(double x, bool missing);

    std::ostream& format(std::ostream&, size_t) const;

    void printHeader(SQLSelect&);

private:  // methods (overrides)
    virtual void print(std::ostream&) const override;

    virtual void reset() override;
    virtual void flush() override;
    virtual bool output(const expression::Expressions&) override;
    virtual void prepare(SQLSelect&) override;
    virtual void updateTypes(SQLSelect&) override;
    virtual void cleanup(SQLSelect&) override;
    virtual unsigned long long count() override;

    virtual void outputReal(double, bool) override;
    virtual void outputDouble(double, bool) override;
    virtual void outputInt(double, bool) override;
    virtual void outputUnsignedInt(double, bool) override;
    virtual void outputString(const char*, size_t, bool) override;
    virtual void outputBitfield(double, bool) override;

private:  // members
    std::ostream& out_;
    unsigned long long count_;
    std::vector<size_t> columnWidths_;
    typedef std::ios_base& (*manipulator)(std::ios_base&);
    std::vector<manipulator> columnAlignments_;
    size_t currentColumn_;

    const SQLOutputConfig& config_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit

#endif
