/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLDistinctOutput..h
// Baudouin Raoult - ECMWF Dec 03

#ifndef SQLDistinctOutput_H
#define SQLDistinctOutput_H

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLOutput.h"

namespace eckit {
namespace sql {

class SQLDistinctOutput : public SQLOutput {
public:
	SQLDistinctOutput(SQLOutput* output);
	virtual ~SQLDistinctOutput(); 

protected:
	virtual void print(std::ostream&) const; 	
private:
// No copy allowed
	SQLDistinctOutput(const SQLDistinctOutput&);
	SQLDistinctOutput& operator=(const SQLDistinctOutput&);

	virtual const SQLOutputConfig& config();
	virtual	void config(SQLOutputConfig&);
// -- Members
    std::auto_ptr<SQLOutput>   output_;
	std::set<std::vector<double> >  seen_;
	std::vector<double>        tmp_;
// -- Overridden methods
	virtual void size(int);
	virtual void reset();
    virtual void flush();
    virtual bool output(const expression::Expressions&);
	virtual void prepare(SQLSelect&);
	virtual void cleanup(SQLSelect&);
	virtual unsigned long long count();

	virtual void outputReal(double, bool) { NOTIMP; };
	virtual void outputDouble(double, bool) { NOTIMP; };
	virtual void outputInt(double, bool) { NOTIMP; };
	virtual void outputUnsignedInt(double, bool) { NOTIMP; };
	virtual void outputString(double, bool) { NOTIMP; };
	virtual void outputBitfield(double, bool) { NOTIMP; };
};

} // namespace sql 
} // namespace eckit 

#endif
