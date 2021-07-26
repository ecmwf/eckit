/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/type/SQLBitfield.h"

#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/SQLTypedefs.h"
#include "eckit/sql/type/SQLBit.h"
#include "eckit/types/Types.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

using namespace eckit;

namespace eckit {
namespace sql {
namespace type {

SQLBitfield::SQLBitfield(const std::string& name, const FieldNames& fields, const Sizes& sizes) :
    SQLType(name),
    bitfieldDef_(make_pair(fields, sizes)) {
    int shift = 0;
    for (size_t i = 0; i < fields.size(); i++) {
        shift_[fields[i]] = shift;
        mask_[fields[i]]  = makeMask(sizes[i]) << shift;
        shift += sizes[i];
    }
    width_ = shift;
}

SQLBitfield::~SQLBitfield() {}

unsigned long SQLBitfield::mask(const std::string& n) const {
    std::map<std::string, unsigned long>::const_iterator j = mask_.find(n);

    if (j == mask_.end())
        throw eckit::UserError("SQLBitfield no field", n);

    return (*j).second;
}

unsigned long SQLBitfield::shift(const std::string& n) const {
    std::map<std::string, unsigned long>::const_iterator j = shift_.find(n);

    if (j == shift_.end())
        throw eckit::UserError("SQLBitfield no field", n);

    return (*j).second;
}

std::string SQLBitfield::make(const std::string& name, const FieldNames& fields, const Sizes& sizes,
                              const char* ddlName) {

    std::stringstream s;
    s << name << "[";
    for (size_t i = 0; i < fields.size(); ++i)
        s << fields[i] << ":" << Translator<int, std::string>()(sizes[i]) << ((i + 1 != fields.size()) ? ";" : "");
    s << "]";
    std::string typeName = s.str();

    if (!exists(typeName)) {
        // Ownership of SQLBitfield assumed by TypeRegistry
        SQLType::registerType(new SQLBitfield(typeName, fields, sizes));
        if (ddlName)
            SQLType::createAlias(typeName, ddlName);
    }

    return typeName;
}

size_t SQLBitfield::width() const {
    return width_;
}

size_t SQLBitfield::size() const {
    return sizeof(long);
}

void SQLBitfield::output(SQLOutput& o, double d, bool missing) const {
    o.outputBitfield(d, missing);
}

std::string SQLBitfield::asString(const double* val) const {
    return eckit::Translator<long, std::string>()(*val);
}

const SQLType* SQLBitfield::subType(const std::string& name) const {
    std::vector<std::string> v;
    Tokenizer(".@")(name, v);

    if (v.size() == 1)
        return this;
    if (v.size() == 2 && name.find('@') != std::string::npos)
        return this;

    ASSERT(v.size() == 3 || v.size() == 2);  // name was e.g: "status.active@body" or "status.active"

    std::string field = v[1];
    std::string full  = name;  // this->name() + "." + field;

    if (exists(full))
        return &lookup(full);

    return SQLType::registerType(new SQLBit(full, mask(field), shift(field)));
}


unsigned long SQLBitfield::makeMask(unsigned long size) {
    unsigned long mask = 0;
    while (size--) {
        mask <<= 1;
        mask |= 1;
    }
    return mask;
}


}  // namespace type
}  // namespace sql
}  // namespace eckit
