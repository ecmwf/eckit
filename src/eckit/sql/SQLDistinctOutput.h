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
/// @date Dec 2003
/// @date Aug 2018

#ifndef eckit_sql_SQLDistinctOutput_H
#define eckit_sql_SQLDistinctOutput_H


#include "eckit/sql/SQLOutput.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLDistinctOutput : public SQLOutput {

    // Comparator to ensure that all cases are well defined, even if we are representing non-double
    // data as doubles

    struct DoubleBitwiseLessComparator {
        bool operator()(const std::vector<double>& lhs, const std::vector<double>& rhs) const {
            if (lhs.size() != rhs.size()) {
                return lhs.size() < rhs.size();
            }
            for (int i = 0; i < lhs.size(); ++i) {
                const void* p1          = &lhs[i];
                const void* p2          = &rhs[i];
                const uint64_t& punned1 = *reinterpret_cast<const uint64_t*>(p1);
                const uint64_t& punned2 = *reinterpret_cast<const uint64_t*>(p2);
                if (punned1 != punned2) {
                    return punned1 < punned2;
                }
            }
            return false;
        }
    };

public:  // methods

    SQLDistinctOutput(SQLOutput& output);
    ~SQLDistinctOutput() override;

private:  // methods

    void print(std::ostream&) const override;

    // -- Members

    SQLOutput& output_;
    std::set<std::vector<double>, DoubleBitwiseLessComparator> seen_;
    std::vector<double> tmp_;
    std::vector<size_t> offsets_;

    // -- Overridden methods
    void reset() override;
    void flush() override;
    bool cachedNext() override;
    bool output(const expression::Expressions&) override;
    void preprepare(SQLSelect&) override;
    void prepare(SQLSelect&) override;
    void updateTypes(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    unsigned long long count() override;

    // Overridden (and removed) functions

    void outputReal(double, bool) override;
    void outputDouble(double, bool) override;
    void outputInt(double, bool) override;
    void outputUnsignedInt(double, bool) override;
    void outputString(const char*, size_t, bool) override;
    void outputBitfield(double, bool) override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
