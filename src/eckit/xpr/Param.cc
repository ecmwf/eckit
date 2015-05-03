/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "Param.h"

#include <sstream>
#include <string>

#include "eckit/xpr/Scope.h"

namespace eckit {
namespace xpr {

Param::Param(const std::string& name) : Expression(), name_(name) {}

Param::Param(Stream& s) : Expression(s), name_() { s >> name_; }

Param::~Param() {}

void Param::encode(Stream& s) const {

  Expression::encode(s);
  s << name_;

}

void Param::print(std::ostream& o) const { o << "_(" << name_ << ")"; }

ExpPtr parameter(const std::string& name) { return ExpPtr(new Param(name)); }

ExpPtr Param::evaluate(Scope& ctx) const {
  ExpPtr e = ctx.param(name_);
  return e->eval();
}

void Param::asCode(std::ostream& o) const { o << factoryName() << "(\"" << name_ << "\")"; }

ExpPtr Param::cloneWith(args_t& a) const {
  NOTIMP;  // Should not be called
}

//--------------------------------------------------------------------------------------------

ClassSpec Param::classSpec_ = {
    &Expression::classSpec(), Param::nodeName(),
};

Reanimator<Param> Param::reanimator_;

//--------------------------------------------------------------------------------------------

}  // namespace xpr
}  // namespace eckit
