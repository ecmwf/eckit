/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @author Simon Smart
/// ECMWF Dec 2012


#ifndef eckit_sql_ShiftedColumnExpression_H
#define eckit_sql_ShiftedColumnExpression_H

#include <list>
#include <string>

namespace eckit {
namespace sql {

class SQLOutput;
class SQLTable;
class SQLSelect;

namespace expression {

//----------------------------------------------------------------------------------------------------------------------

template <typename T>
class ShiftedColumnExpression : public T {
public:

    ShiftedColumnExpression(const std::string&, SQLTable*, int shift, int nominalShift, int begin = -1, int end = -1);
    ShiftedColumnExpression(const std::string&, const std::string& tableReference, int shift, int nominalShift,
                            int begin = -1, int end = -1);
    ShiftedColumnExpression(const ShiftedColumnExpression&);
    ShiftedColumnExpression(const T& o, int shift, int nominalShift);

    // for bitfields columns
    ShiftedColumnExpression(const std::string& name, const std::string& field, SQLTable* table, int shift,
                            int nominalShift);
    ShiftedColumnExpression(const std::string& name, const std::string& field, const std::string& tableReference,
                            int shift, int nominalShift);


    ~ShiftedColumnExpression();

    SQLTable* table();

    double* current();

    std::shared_ptr<SQLExpression> clone() const override;
    std::shared_ptr<SQLExpression> reshift(int minColumnShift) const override;

    int shift() const { return shift_; }
    int nominalShift() const { return nominalShift_; }

protected:

    int shift_;         // For the HASH operator
    int nominalShift_;  // For the HASH operator

    // -- Overridden methods
    void print(std::ostream& s) const override;
    void cleanup(SQLSelect& sql) override;
    using T::eval;
    double eval(bool& missing) const override;
    void output(SQLOutput& s) const override;

private:

    ShiftedColumnExpression& operator=(const ShiftedColumnExpression&);

    void allocateCircularBuffer();
    std::list<std::pair<double, bool> > oldValues_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace expression
}  // namespace sql
}  // namespace eckit

#include "eckit/sql/expression/ShiftedColumnExpression.cc"

#endif
