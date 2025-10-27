/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FunctionSTDEV.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef FunctionSTDEV_H
#define FunctionSTDEV_H

#include "eckit/sql/expression/function/FunctionVAR.h"

namespace eckit::sql::expression::function {

class FunctionSTDEV : public FunctionVAR {
public:

    FunctionSTDEV(const std::string&, const expression::Expressions&);
    FunctionSTDEV(const FunctionSTDEV&);
    ~FunctionSTDEV();

    std::shared_ptr<SQLExpression> clone() const override;

private:

    // No copy allowed
    FunctionSTDEV& operator=(const FunctionSTDEV&);

    using FunctionVAR::eval;
    double eval(bool& missing) const override;
    const eckit::sql::type::SQLType* type() const override;

    // -- Friends
    // friend std::ostream& operator<<(std::ostream& s,const FunctionSTDEV& p)
    //	{ p.print(s); return s; }
};

}  // namespace eckit::sql::expression::function

#endif
