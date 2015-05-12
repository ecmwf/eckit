/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Matrix.h
/// @author Florian Rathgeber
/// @date April 2015

#ifndef eckit_xpr_Matrix_h
#define eckit_xpr_Matrix_h

#include <functional>

#include "eckit/xpr/Value.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class Matrix : public Value {

public: // types

    typedef real_t elemt_t;
    typedef std::vector<elemt_t> value_t;

public: // methods

    static const char * nodeName() { return "xpr::Matrix"; }

    static std::string sig() { return "m"; }

    static bool is ( const ExpPtr& e );

    static const value_t& extract ( const ExpPtr& e )
    {
        ASSERT( Matrix::is(e) );
        return e->as<Matrix>()->value();
    }

    static const size_t rows( const ExpPtr& e )
    {
        ASSERT( Matrix::is(e) );
        return e->as<Matrix>()->rows();
    }

    static const size_t cols( const ExpPtr& e )
    {
        ASSERT( Matrix::is(e) );
        return e->as<Matrix>()->cols();
    }

    Matrix( const size_t& rows, const size_t& cols, const real_t& v = real_t() );
    Matrix( const size_t& rows, const size_t& cols, const value_t& v );
    Matrix( const size_t& rows, const size_t& cols, value_t& v, Swap );
    Matrix( const size_t& rows, const size_t& cols,
            const std::function< real_t(size_t, size_t) >& );

    Matrix( Matrix&& ) = default;

    Matrix( Stream& s );

    Matrix& operator=(Matrix&&) = default;

    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }

    /// @returns a copy of the internal vector
    const value_t& value() const { return v_; }

    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
    static const ClassSpec& classSpec() { return classSpec_; }

public: // virtual methods

    virtual std::string factoryName() const { return "xpr::matrix"; }
    virtual const char * typeName() const { return nodeName(); }
    virtual std::string signature() const { return sig(); }

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& o ) const;
    virtual void asJSON( JSON& s ) const;
    virtual ExpPtr cloneWith(args_t& a) const;

protected: // virtual methods

    // From Streamable
    virtual void encode( Stream& s ) const;

protected: // members

    value_t v_;
    size_t rows_;
    size_t cols_;

private: // static members

    static  ClassSpec classSpec_;
    static  Reanimator<Matrix> reanimator_;
};

//--------------------------------------------------------------------------------------------

ExpPtr matrix( const size_t& rows, const size_t& cols, const real_t& v = real_t() );
ExpPtr matrix( const size_t& rows, const size_t& cols, const Matrix::value_t& v  );
ExpPtr matrix( const size_t& rows, const size_t& cols, const std::initializer_list<real_t> v );
ExpPtr matrix( const size_t& rows, const size_t& cols, const std::function< real_t(size_t, size_t) >& );

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
