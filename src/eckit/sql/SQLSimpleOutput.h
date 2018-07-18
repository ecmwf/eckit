/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLSimpleOutput.h
/// Baudouin Raoult - ECMWF Dec 03

#ifndef odb_api_SQLSimpleOutput_H
#define odb_api_SQLSimpleOutput_H

#include "eckit/sql/SQLOutput.h"

namespace eckit {
namespace sql {

class SQLSimpleOutput : public SQLOutput {
public:
    SQLSimpleOutput(std::ostream& = std::cout);
	virtual ~SQLSimpleOutput(); 

protected:
	virtual void print(std::ostream&) const; 	

private:
	SQLSimpleOutput(const SQLSimpleOutput&);
	SQLSimpleOutput& operator=(const SQLSimpleOutput&);

    std::ostream& out_;
	unsigned long long count_;
	std::vector<size_t> columnWidths_;
	typedef std::ios_base& (*manipulator)(std::ios_base&);
	std::vector<manipulator> columnAlignments_;
	size_t currentColumn_;

    std::ostream& format(std::ostream&, size_t) const;

	void printHeader(SQLSelect&);

// -- Overridden methods
	virtual void size(int);
	virtual void reset();
    virtual void flush();
    virtual bool output(const expression::Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

    template <typename T> void outputValue(double x, bool missing);

	virtual void outputReal(double, bool);
	virtual void outputDouble(double, bool);
	virtual void outputInt(double, bool);
	virtual void outputUnsignedInt(double, bool);
	virtual void outputString(double, bool);
	virtual void outputBitfield(double, bool);
};

} // namespace sql
} // namespace eckit

#endif
