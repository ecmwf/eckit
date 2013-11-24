/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file List.h
/// @author Tiago Quintino
/// @date November 2013

#ifndef eckit_maths_List_h
#define eckit_maths_List_h

#include "eckit/maths/Value.h"

namespace eckit {
namespace maths {

//--------------------------------------------------------------------------------------------

class List : public Value {

public: // types

    typedef args_t value_t;

public: // methods

    static std::string className() { return "List"; }

    static std::string sig() { return "l"; }

    static bool is ( const ExpPtr& e );

    static value_t& extract ( const ExpPtr& e )
    {
        assert( List::is(e) );
        return e->as<List>()->ref_value();
    }

    List();
    List( ExpPtr e );
    List( const ListPtr& l );
    List( const value_t& v );

    /// @returns the size of the internal vector
    value_t value() const { return args_; }
    /// @returns a reference to the internal vector
    value_t& ref_value() { return args_; }

    /// Append a list to
    ListPtr append( const List& l );

    /// Append an element to the list
    ListPtr append( ExpPtr e );

    /// Append multiple elements to the list
    template <class Arg1, class ...ArgN>
    ListPtr append(const Arg1& a1, const ArgN& ...an)
    {
        append(a1);
        append(an...);
        return as<List>();
    }

private: // virtual methods

    virtual std::string typeName() const { return List::className(); }
    virtual std::string signature() const { return List::sig(); }
    virtual std::string returnSignature() const { return List::sig(); }

    virtual ExpPtr clone();

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& ) const;

};

typedef boost::shared_ptr< List > ListPtr;

//--------------------------------------------------------------------------------------------

ListPtr list();
ListPtr list( const List::value_t& v  );
ListPtr list( ExpPtr e );
ListPtr list( ExpPtr v0, ExpPtr v1 );

template <class Arg1, class ...ArgN>
ListPtr list(const Arg1& a1, const ArgN& ...an)
{
    return maths::list()->append(a1,an...);
}

//--------------------------------------------------------------------------------------------

} // namespace maths
} // namespace eckit

#endif
