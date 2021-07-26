/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>
#include <iomanip>
#include <set>

#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/sql/expression/function/FunctionFactory.h"
#include "eckit/testing/Test.h"

using namespace eckit::testing;
using namespace eckit::types;
using eckit::Log;

namespace {

//----------------------------------------------------------------------------------------------------------------------

static const std::vector<long> INTEGER_DATA{9999, 8888, 7777, 6666, 5555, 4444, 3333, 2222, 1111};
static const std::vector<double> REAL_DATA{99.9, 88.8, 77.7, 66.6, 55.5, 44.4, 33.3, 22.2, 11.1};
static const std::vector<std::string> STRING_DATA{
    "aaaabbbb", "cccc", "dddd", "eeffg", "hijklmno", "a-string", "a-longer-string", "another-string", ""};
static const std::vector<double> REAL_DATA2{3.0, 4.0, -12.3, 93.7, 11.11, 3.1415, -5.4321, 1.0, -2.0};

static const std::vector<long> DATE_DATA{20210616, 0, 99991231, 100000000, 20210615, 20210614, 20210613, 20210612, 20210611};
static const std::vector<long> TIME_DATA{123456, 123456, 123456, 123456, 0, 654321, 6543, 1000000, 114335};

static const std::vector<long> DATE_DATA2{20210615, 20210614, 20210613, 20210612, 20210611, 20210616, 0, 99991231, 100000000};
static const std::vector<long> TIME_DATA2{0, 654321, 6543, 1000000, 114335, 123456, 123456, 123456, 123456};

class TestTable : public eckit::sql::SQLTable {

public:
    TestTable(eckit::sql::SQLDatabase& db, const std::string& path, const std::string& name) :
        SQLTable(db, path, name) {
        addColumn("icol", 0, eckit::sql::type::SQLType::lookup("integer"), false, 0);
        addColumn("scol", 1, eckit::sql::type::SQLType::lookup("string", 2), false, 0);
        addColumn("rcol", 2, eckit::sql::type::SQLType::lookup("real"), false, 0);
        addColumn("rcol2", 3, eckit::sql::type::SQLType::lookup("real"), false, 0);
        addColumn("dates", 4, eckit::sql::type::SQLType::lookup("integer"), false, 0);
        addColumn("times", 5, eckit::sql::type::SQLType::lookup("integer"), false, 0);
        addColumn("dates2", 6, eckit::sql::type::SQLType::lookup("integer"), false, 0);
        addColumn("times2", 7, eckit::sql::type::SQLType::lookup("integer"), false, 0);
    }

private:
    class TestTableIterator : public eckit::sql::SQLTableIterator {
    public:
        TestTableIterator(const TestTable& owner,
                          const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns) :
            /* owner_(owner), */ idx_(0), data_(9) {
            std::vector<size_t> offsets{0, 1, 3, 4, 5, 6, 7, 8};
            std::vector<size_t> doublesSizes{1, 2, 1, 1, 1, 1, 1, 1};
            for (const auto& col : columns) {
                offsets_.push_back(offsets[col.get().index()]);
                doublesSizes_.push_back(doublesSizes[col.get().index()]);
                hasMissing_.push_back(false);
                missingVals_.push_back(0);
            }
        }

    private:
        ~TestTableIterator() override {}
        void rewind() override { idx_ = 0; }
        bool next() override {
            if (idx_ < INTEGER_DATA.size()) {
                copyRow();
                idx_++;
                return true;
            }
            return false;
        }
        void copyRow() {
            data_[0] = INTEGER_DATA[idx_];
            ::strncpy(reinterpret_cast<char*>(&data_[1]), STRING_DATA[idx_].c_str(), 16);
            data_[3] = REAL_DATA[idx_];
            data_[4] = REAL_DATA2[idx_];
            data_[5] = DATE_DATA[idx_];
            data_[6] = TIME_DATA[idx_];
            data_[7] = DATE_DATA2[idx_];
            data_[8] = TIME_DATA2[idx_];
        }
        std::vector<size_t> columnOffsets() const override { return offsets_; }
        std::vector<size_t> doublesDataSizes() const override { return doublesSizes_; }
        std::vector<char> columnsHaveMissing() const override { return hasMissing_; }
        std::vector<double> missingValues() const override { return missingVals_; }
        const double* data() const override { return &data_[0]; }

        // const TestTable& owner_; // unused
        size_t idx_;
        std::vector<size_t> offsets_;
        std::vector<size_t> doublesSizes_;
        std::vector<double> data_;
        std::vector<char> hasMissing_;
        std::vector<double> missingVals_;
    };

    eckit::sql::SQLTableIterator* iterator(
        const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns,
        std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback) const override {
        // TODO: Test callback
        return new TestTableIterator(*this, columns);
    }
};


//----------------------------------------------------------------------------------------------------------------------

class TestOutput : public eckit::sql::SQLOutput {

    virtual void prepare(eckit::sql::SQLSelect&) {}
    virtual void cleanup(eckit::sql::SQLSelect&) {}
    virtual void reset() {}
    virtual void flush() {}

    virtual bool output(const eckit::sql::expression::Expressions& results) {
        for (const auto& r : results)
            r->output(*this);
        return true;
    };

    virtual void outputReal(double d, bool) { floatOutput.push_back(d); }
    virtual void outputDouble(double d, bool) { floatOutput.push_back(d); }
    virtual void outputInt(double d, bool missing) {
        if (missing) {
            intOutput.push_back(-1);
        }
        else {
            intOutput.push_back(d);
        }
    }
    virtual void outputUnsignedInt(double d, bool) { intOutput.push_back(d); }
    virtual void outputString(const char* s, size_t l, bool) { strOutput.push_back(std::string(s, l)); }
    virtual void outputBitfield(double d, bool) { intOutput.push_back(d); }

    virtual unsigned long long count() {
        return std::max(intOutput.size(), std::max(floatOutput.size(), strOutput.size()));
    };

public:  // visible members
    std::vector<long> intOutput;
    std::vector<double> floatOutput;
    std::vector<std::string> strOutput;
};

//----------------------------------------------------------------------------------------------------------------------


CASE("Test available SQL functions") {

    // Test that the functions provided as part of eckit are found

    std::set<std::pair<std::string, int>> functions{{"+", 2},
                                                    {"-", 1},  // Unary negative
                                                    {"-", 2},
                                                    {"/", 2},
                                                    {"*", 2},
                                                    {"<", 2},
                                                    {">", 2},
                                                    {"<=", 2},
                                                    {">=", 2},
                                                    {"<>", 2},
                                                    {"=", 2},
                                                    {"abs", 1},
                                                    {"acos", 1},
                                                    {"and", 2},
                                                    {"asin", 1},
                                                    {"atan", 1},
                                                    {"atan2", 2},
                                                    {"atoi", 1},
                                                    {"avg", 1},
                                                    {"between", 3},
                                                    {"between_exclude_both", 3},
                                                    {"between_exclude_second", 3},
                                                    {"c2f", 1},
                                                    {"c2k", 1},
                                                    {"ceil", 1},
                                                    {"celsius", 1},
                                                    {"circle", 5},
                                                    {"cos", 1},
                                                    {"cosh", 1},
                                                    {"count", 1},  // Aggregate fn
                                                    {"day", 1},
                                                    {"dd", 2},
                                                    {"deg2rad", 1},
                                                    {"degrees", 1},
                                                    {"dir", 2},
                                                    {"direction", 2},
                                                    {"dist", 5},
                                                    {"distance", 4},
                                                    {"dotp", 2},
                                                    {"exp", 1},
                                                    {"f2c", 1},
                                                    {"f2k", 1},
                                                    {"fabs", 1},
                                                    {"fahrenheit", 1},
                                                    {"ff", 2},
                                                    {"first", 1},
                                                    {"floor", 1},
                                                    {"fmod", 2},
                                                    {"hour", 1},
                                                    {"ibits", 3},
                                                    {"in", -1},
                                                    {"int", 1},
                                                    {"isnull", 1},
                                                    {"jd", 2},
                                                    {"join", 2},
                                                    {"julian", 2},
                                                    {"julian_date", 2},
                                                    {"julian_seconds", 2},
                                                    {"k2c", 1},
                                                    {"k2f", 1},
                                                    {"kelvin", 1},
                                                    {"km", 1},
                                                    {"km", 4},
                                                    {"last", 1},
                                                    {"ldexp_double", 2},
                                                    {"lg", 1},
                                                    {"like", 2},
                                                    {"ln", 1},
                                                    {"log", 1},
                                                    {"log10", 1},
                                                    {"max", 1},
                                                    {"mean", 1},
                                                    {"min", 1},
                                                    {"minute", 1},
                                                    {"minutes", 1},
                                                    {"mod", 2},
                                                    {"month", 1},
                                                    {"nint", 1},
                                                    {"norm", 2},
                                                    {"not", 1},
                                                    {"not_between", 3},
                                                    {"not_in", -1},
                                                    {"not_null", 1},
                                                    {"null", 1},
                                                    {"nvl", 2},
                                                    {"or", 2},
                                                    {"pow", 2},
                                                    {"rad", 5},
                                                    {"rad2deg", 1},
                                                    {"radians", 1},
                                                    {"rlike", 2},
                                                    {"rms", 1},
                                                    {"rownumber", 0},
                                                    {"second", 1},
                                                    {"seconds", 1},
                                                    {"sin", 1},
                                                    {"sinh", 1},
                                                    {"speed", 2},
                                                    {"sqrt", 1},
                                                    {"stddev", 1},
                                                    {"stdev", 1},
                                                    {"stdevp", 1},
                                                    {"sum", 1},
                                                    {"tan", 1},
                                                    {"tanh", 1},
                                                    {"tdiff", 4},
                                                    {"thin", 2},
                                                    {"timestamp", 2},
                                                    {"trunc", 1},
                                                    {"twice", 1},
                                                    {"var", 1},
                                                    {"varp", 1},
                                                    {"year", 1}};

    auto info = eckit::sql::expression::function::FunctionFactory::instance().functionsInfo();

    for (const auto& i : info) {
        auto it = functions.find(std::make_pair(i.name, i.arity));
        if (it != functions.end()) {
            functions.erase(it);
        }
        else {
            Log::info() << "Not matching: " << i.name << ": " << i.arity << std::endl;
        }
    }

    if (!functions.empty()) {
        Log::error() << "Not all functions found: " << std::endl;
        for (const auto& f : functions) {
            Log::error() << "  name: " << f.first << ", arity: " << f.second << std::endl;
        }
    }

    EXPECT(functions.empty());
}


CASE("Functions using test data") {

    // Construct session with TestOutput to get the output

    eckit::sql::SQLSession session(std::unique_ptr<TestOutput>(new TestOutput));
    eckit::sql::SQLDatabase& db(session.currentDatabase());
    db.addTable(new TestTable(db, "a/b/c.path", "table1"));

    SECTION("Test SQL comparisons") {

        std::string sql = "select icol > 3333, icol >= 3333 from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 0);
        EXPECT(o.strOutput.size() == 0);
        EXPECT(o.floatOutput.size() == 18);
        for (size_t i = 0; i < 12; i++)
            EXPECT(o.floatOutput[i]);
        EXPECT(!o.floatOutput[12]);
        EXPECT(o.floatOutput[13]);
        for (size_t i = 14; i < 18; i++)
            EXPECT(!o.floatOutput[i]);
    }

    SECTION("Test simple arithmetic -- addition/subtraction") {

        std::string sql = "select icol + 15.3, rcol + 13.5, icol + rcol, icol - 15.3, rcol - 13.5, icol - rcol from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 0);
        EXPECT(o.floatOutput.size() == 54);
        EXPECT(o.strOutput.size() == 0);

        // i = [9999, 8888, 7777, 6666, 5555, 4444, 3333, 2222, 1111]
        // r = [99.9, 88.8, 77.7, 66.6, 55.5, 44.4, 33.3, 22.2, 11.1]
        // for ii, rr in zip(i, r):
        //    print ", ".join("{:7.1f}".format(v) for v in (ii+15.3, rr+13.5, ii+rr, ii-15.3, rr-13.5, ii-rr))

        std::vector<double> expected{
            10014.3,
            113.4,
            10098.9,
            9983.7,
            86.4,
            9899.1,
            8903.3,
            102.3,
            8976.8,
            8872.7,
            75.3,
            8799.2,
            7792.3,
            91.2,
            7854.7,
            7761.7,
            64.2,
            7699.3,
            6681.3,
            80.1,
            6732.6,
            6650.7,
            53.1,
            6599.4,
            5570.3,
            69.0,
            5610.5,
            5539.7,
            42.0,
            5499.5,
            4459.3,
            57.9,
            4488.4,
            4428.7,
            30.9,
            4399.6,
            3348.3,
            46.8,
            3366.3,
            3317.7,
            19.8,
            3299.7,
            2237.3,
            35.7,
            2244.2,
            2206.7,
            8.7,
            2199.8,
            1126.3,
            24.6,
            1122.1,
            1095.7,
            -2.4,
            1099.9,
        };

        EXPECT(is_approximately_equal(o.floatOutput, expected, 0.000001));
    }

    SECTION("Test simple arithmetic -- multiplication/division") {

        std::string sql = "select icol * 15.3, rcol * 13.5, icol * rcol, icol / 15.3, rcol / 13.5, icol / rcol from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 0);
        EXPECT(o.floatOutput.size() == 54);
        EXPECT(o.strOutput.size() == 0);

        // i = [9999, 8888, 7777, 6666, 5555, 4444, 3333, 2222, 1111]
        // r = [99.9, 88.8, 77.7, 66.6, 55.5, 44.4, 33.3, 22.2, 11.1]
        // for ii, rr in zip(i, r):
        //     print ", ".join("{:7.4f}".format(v) for v in (ii*15.3, rr*13.5, ii*rr, ii/15.3, rr/13.5, ii/rr))

        std::vector<double> expected{
            152984.7000, 1348.6500, 998900.1000, 653.5294, 7.4000, 100.0901, 135986.4000, 1198.8000,
            789254.4000, 580.9150, 6.5778, 100.0901, 118988.1000, 1048.9500, 604272.9000, 508.3007,
            5.7556, 100.0901, 101989.8000, 899.1000, 443955.6000, 435.6863, 4.9333, 100.0901,
            84991.5000, 749.2500, 308302.5000, 363.0719, 4.1111, 100.0901, 67993.2000, 599.4000,
            197313.6000, 290.4575, 3.2889, 100.0901, 50994.9000, 449.5500, 110988.9000, 217.8431,
            2.4667, 100.0901, 33996.6000, 299.7000, 49328.4000, 145.2288, 1.6444, 100.0901,
            16998.3000, 149.8500, 12332.1000, 72.6144, 0.8222, 100.0901};

        EXPECT(is_approximately_equal(o.floatOutput, expected, 0.0001));
    }

    SECTION("Test SQL conditional composition AND") {

        std::string sql = "select icol > 3333 and rcol <= 66.6 from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 0);
        EXPECT(o.floatOutput.size() == 9);
        EXPECT(o.strOutput.size() == 0);

        for (size_t i = 0; i < 3; i++)
            EXPECT(!o.floatOutput[i]);
        for (size_t i = 3; i < 5; i++)
            EXPECT(o.floatOutput[i]);
        for (size_t i = 6; i < 9; i++)
            EXPECT(!o.floatOutput[i]);
    }

    SECTION("Test SQL conditional composition OR") {

        std::string sql = "select icol <= 3333 or rcol > 66.6 from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 0);
        EXPECT(o.floatOutput.size() == 9);
        EXPECT(o.strOutput.size() == 0);

        for (size_t i = 0; i < 3; i++)
            EXPECT(o.floatOutput[i]);
        for (size_t i = 3; i < 5; i++)
            EXPECT(!o.floatOutput[i]);
        for (size_t i = 6; i < 9; i++)
            EXPECT(o.floatOutput[i]);
    }

    SECTION("Test SQL conditional composition NOT OR") {

        std::string sql = "select not (icol <= 3333 or rcol > 66.6) from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 0);
        EXPECT(o.floatOutput.size() == 9);
        EXPECT(o.strOutput.size() == 0);

        for (size_t i = 0; i < 3; i++)
            EXPECT(!o.floatOutput[i]);
        for (size_t i = 3; i < 5; i++)
            EXPECT(o.floatOutput[i]);
        for (size_t i = 6; i < 9; i++)
            EXPECT(!o.floatOutput[i]);
    }

    SECTION("Test SQL aggregates") {

        std::string sql = "select count(*), count(icol), mean(icol), sum(icol) from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 0);
        EXPECT(o.floatOutput.size() == 4);
        EXPECT(o.strOutput.size() == 0);

        std::vector<double> expected{9, 9, 5555, 49995};
        EXPECT(o.floatOutput == expected);
    }

    SECTION("Test SQL rownumber() with other column") {

        std::string sql = "select icol,rownumber() from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 18);
        EXPECT(o.floatOutput.size() == 0);
        EXPECT(o.strOutput.size() == 0);

        ASSERT(INTEGER_DATA.size() == 9);
        std::vector<long> expectedInt;
        for (int i = 0; i < 9; ++i) {
            expectedInt.push_back(INTEGER_DATA[i]);
            expectedInt.push_back(i + 1);
        }
        EXPECT(o.intOutput == expectedInt);
    }

    SECTION("Test SQL rownumber() alone") {

        std::string sql = "select rownumber() from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        Log::info() << "intOutput size: " << o.intOutput.size() << std::endl;
        EXPECT(o.intOutput.size() == 9);
        EXPECT(o.floatOutput.size() == 0);
        EXPECT(o.strOutput.size() == 0);

        std::vector<long> expectedInt{1, 2, 3, 4, 5, 6, 7, 8, 9};
        EXPECT(o.intOutput == expectedInt);
    }

    SECTION("Test dotp()") {

        std::string sql = "select dotp(rcol, rcol2) from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 0);
        EXPECT(o.floatOutput.size() == 1);
        EXPECT(o.strOutput.size() == 0);

        EXPECT(is_approximately_equal(o.floatOutput[0], 6514.80867, 1e-7));
    }

    SECTION("Test TIMESTAMP()") {
        std::string sql = "select timestamp(dates, times) from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        Log::info() << "intOutput(): " << o.intOutput << std::endl;
        EXPECT(o.intOutput.size() == 9);
        EXPECT(o.floatOutput.size() == 0);
        EXPECT(o.strOutput.size() == 0);

        std::vector<long> expectedInt{
            20210616123456,
            123456,
            99991231123456,
            -1,
            20210615000000,
            -1,
            20210613006543,
            -1,
            20210611114335};
        EXPECT(o.intOutput == expectedInt);
    }

    SECTION("Test TDIFF()") {
        std::string sql = "select tdiff(dates, times, dates2, times2) from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();
        TestOutput& o(static_cast<TestOutput&>(session.output()));

        EXPECT(o.intOutput.size() == 9);
        EXPECT(o.floatOutput.size() == 0);
        EXPECT(o.strOutput.size() == 0);

        std::vector<long> expectedInt{
            131696,
            -1,
            -1,
            -1,
            303385,
            -1,
            -1,
            -1,
            -1};
        EXPECT(o.intOutput == expectedInt);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
