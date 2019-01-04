/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/expression/function/EqRegionCache.h"
#include "eckit/sql/expression/function/FunctionEQ_BOXLAT.h"
#include "eckit/sql/expression/function/FunctionFactory.h"

namespace eckit {
namespace sql {
namespace expression {
namespace function {

/* Static self-registration */

static FunctionBuilder<FunctionEQ_BOXLAT> eq_boxlatFunctionBuilder("eq_boxlat");

const type::SQLType* FunctionEQ_BOXLAT::type() const { return &type::SQLType::lookup("real"); }

FunctionEQ_BOXLAT::FunctionEQ_BOXLAT(const std::string& name, const expression::Expressions& args)
: FunctionExpression(name, args)
{}

FunctionEQ_BOXLAT::FunctionEQ_BOXLAT(const FunctionEQ_BOXLAT& other)
: FunctionExpression(other.name_, other.args_)
{}

FunctionEQ_BOXLAT::~FunctionEQ_BOXLAT() {}

std::shared_ptr<SQLExpression> FunctionEQ_BOXLAT::clone() const { return std::make_shared<FunctionEQ_BOXLAT>(*this); }

double FunctionEQ_BOXLAT::eval(bool& missing) const
{
    double lat_degrees = args_[0]->eval(missing);
    double lon_degrees = args_[1]->eval(missing);
    lon_degrees = lon_degrees; // variable not used
    double resol = args_[2]->eval(missing);
    double res=0.;
    EqRegionCache p;
    res = p.get_midlat(resol, lat_degrees);
    return res;
}

} // namespace function
} // namespace expression
} // namespace sql
} // namespace eckit
