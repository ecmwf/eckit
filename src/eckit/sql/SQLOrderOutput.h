/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// \file SQLOrderOutput.h
/// Piotr Kuchta - ECMWF Nov 11

#ifndef SQLOrderOutput_H
#define SQLOrderOutput_H

#include "odb_api/Expressions.h"
#include "odb_api/OrderByExpressions.h"
#include "eckit/sql/SQLOutput.h"

namespace eckit {
namespace sql {

class SQLOrderOutput : public SQLOutput {
public:
    SQLOrderOutput(SQLOutput* output, const std::pair<Expressions,std::vector<bool> >& by);
	virtual ~SQLOrderOutput();

protected:
	virtual void print(std::ostream&) const;

private:
// No copy allowed
	SQLOrderOutput(const SQLOrderOutput&);
	SQLOrderOutput& operator=(const SQLOrderOutput&);

// -- Members
    std::auto_ptr<SQLOutput> output_;
	std::pair<Expressions,std::vector<bool> > by_;
	
	typedef std::map<OrderByExpressions, VectorOfExpressions> SortedResults;

	SortedResults sortedResults_;
    std::vector<size_t> byIndices_;

// -- Overridden methods
	virtual void size(int);
	virtual void reset();
    virtual void flush();
    virtual bool output(const Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool) { NOTIMP; };
	virtual void outputDouble(double, bool) { NOTIMP; };
	virtual void outputInt(double, bool) { NOTIMP; };
	virtual void outputUnsignedInt(double, bool) { NOTIMP; };
	virtual void outputString(double, bool) { NOTIMP; };
	virtual void outputBitfield(double, bool) { NOTIMP; };

	virtual const SQLOutputConfig& config();
	virtual void config(SQLOutputConfig&);

	friend std::ostream& operator<<(std::ostream& s, const SQLOrderOutput& o)
		{ o.print(s); return s; }
};

} // namespace sql
} // namespace eckit

#endif
