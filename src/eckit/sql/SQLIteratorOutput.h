// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// \file SQLIteratorOutput.h
/// Piotr Kuchta - ECMWF Feb 09

#ifndef SQLIteratorOutput_H
#define SQLIteratorOutput_H

#include "eckit/exception/Exceptions.h"
#include "eckit/sql/SQLOutput.h"

namespace eckit {

class SelectIterator;

namespace sql {

class ReaderIterator;

template <typename T = odb::SelectIterator>
class SQLIteratorOutput : public SQLOutput {
public:

    SQLIteratorOutput(T&);
    ~SQLIteratorOutput() override;

protected:

    void print(std::ostream&) const override;

private:

    // No copy allowed
    SQLIteratorOutput(const SQLIteratorOutput&);
    SQLIteratorOutput& operator=(const SQLIteratorOutput&);

    // -- Members
    T& iterator_;
    // bool headerSaved = true;
    unsigned long long count_;

    // -- Methods
    // None

    // -- Overridden methods

    virtual void size(int);
    virtual void reset();
    void flush() override;
    virtual bool output(const expression::Expressions&);
    void prepare(SQLSelect&) override;
    void cleanup(SQLSelect&) override;
    virtual unsigned long long count();

    virtual void outputReal(double, bool) { NOTIMP; };
    virtual void outputDouble(double, bool) { NOTIMP; };
    virtual void outputInt(double, bool) { NOTIMP; };
    virtual void outputUnsignedInt(double, bool) { NOTIMP; };
    virtual void outputString(double, bool) { NOTIMP; };
    virtual void outputBitfield(double, bool) { NOTIMP; };
};

}  // namespace sql
}  // namespace eckit

#include "eckit/sql/SQLIteratorOutput.cc"

#endif
