// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
