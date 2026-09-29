// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Simon Smart
/// @date Dec 2003

#ifndef eckit_sql_SQLSimpleOutput_H
#define eckit_sql_SQLSimpleOutput_H

#include "eckit/sql/SQLOutput.h"

namespace eckit::sql {

//----------------------------------------------------------------------------------------------------------------------

class SQLSimpleOutput : public SQLOutput {

public:  // methods

    SQLSimpleOutput(const SQLOutputConfig& config, std::ostream& out);
    ~SQLSimpleOutput() override;

private:  // methods

    template <typename T>
    void outputValue(double x, bool missing);

    std::ostream& format(std::ostream&, size_t) const;

    void printHeader(SQLSelect&);

private:  // methods (overrides)

    void print(std::ostream&) const override;

    void reset() override;
    void flush() override;
    bool output(const expression::Expressions&) override;
    void prepare(SQLSelect&) override;
    void updateTypes(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    unsigned long long count() override;

    void outputReal(double, bool) override;
    void outputDouble(double, bool) override;
    void outputInt(double, bool) override;
    void outputUnsignedInt(double, bool) override;
    void outputString(const char*, size_t, bool) override;
    void outputBitfield(double, bool) override;

private:  // members

    std::ostream& out_;
    unsigned long long count_;
    std::vector<size_t> columnWidths_;
    using manipulator = std::ios_base& (*)(std::ios_base&);
    std::vector<manipulator> columnAlignments_;
    size_t currentColumn_;

    const SQLOutputConfig& config_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
