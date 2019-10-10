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
/// ECMWF Dec 03

#ifndef eckit_sql_SQLSimpleOutput_H
#define eckit_sql_SQLSimpleOutput_H

#include "eckit/sql/SQLOutput.h"

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLSimpleOutput : public SQLOutput {

public: // methods

    SQLSimpleOutput(const SQLOutputConfig& config, std::ostream& out);
	virtual ~SQLSimpleOutput(); 

private: // methods

    template <typename T> void outputValue(double x, bool missing);

    std::ostream& format(std::ostream&, size_t) const;

    void printHeader(SQLSelect&);

private: // methods (overrides)

    virtual void print(std::ostream&) const;

	virtual void reset();
    virtual void flush();
    virtual bool output(const expression::Expressions&);
	virtual void prepare(SQLSelect&);
    virtual void updateTypes(SQLSelect&);
    virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool);
	virtual void outputDouble(double, bool);
	virtual void outputInt(double, bool);
	virtual void outputUnsignedInt(double, bool);
    virtual void outputString(const char*, size_t, bool);
    virtual void outputBitfield(double, bool);

private: // members

    std::ostream& out_;
    unsigned long long count_;
    std::vector<size_t> columnWidths_;
    typedef std::ios_base& (*manipulator)(std::ios_base&);
    std::vector<manipulator> columnAlignments_;
    size_t currentColumn_;

    const SQLOutputConfig& config_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace sql
} // namespace eckit

#endif
