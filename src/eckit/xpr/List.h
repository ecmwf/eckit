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

#ifndef eckit_xpr_List_h
#define eckit_xpr_List_h

#include "eckit/memory/SharedPtr.h"

#include "eckit/xpr/Value.h"

namespace eckit {
namespace xpr {

//--------------------------------------------------------------------------------------------

class List : public Value {

public: // types

    typedef SharedPtr<List> Ptr;
    typedef args_t value_t;

public: // methods

    static std::string className() { return "List"; }

    static std::string sig() { return "l"; }

    static const value_t& extract ( Scope& ctx , const ExpPtr& e )
    {
        return e->as<List>()->value();
    }

    List();
    List( ExpPtr e );
    List(const args_t &args );
    List(args_t &args, Swap );

    /// @returns the size of the internal std::vector
    const value_t& value() const { return args(); }


private: // methods

    virtual std::string typeName() const { return List::className(); }
    virtual std::string signature() const { return List::sig(); }
    virtual std::string returnSignature() const { return List::sig(); }

    virtual void print( std::ostream& o ) const;
    virtual void asCode( std::ostream& ) const;

    virtual ExpPtr cloneWith(args_t& a) const;

    /// Adds support for count()
    virtual bool countable() const { return true; }
    virtual size_t count() const { return size(); }

};

//--------------------------------------------------------------------------------------------

ExpPtr list();
ExpPtr list( const List::value_t& v  );

//--------------------------------------------------------------------------------------------

static void build_list(List::value_t &l) {
    // End of recursion
}

template <typename T, typename ...A>
static void build_list(List::value_t& l, T head, A... tail) {
    l.push_back(head);
    build_list(l, tail...);
}

template <class ...A>
ExpPtr list(A... args)
{
    List::value_t l;
    l.reserve(sizeof...(args));
    build_list(l, args...);
    return ExpPtr(new List(l, List::Swap()));
}

//--------------------------------------------------------------------------------------------

} // namespace xpr
} // namespace eckit

#endif
