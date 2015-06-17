/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   LinearAlgebraFactory.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_la_LinearAlgebraFactory_h
#define eckit_la_LinearAlgebraFactory_h

#include <string>
#include <map>

#include "eckit/memory/NonCopyable.h"

#include "eckit/la/LinearAlgebraBase.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

class LinearAlgebraFactory : private NonCopyable {

public:  // virtual methods
    static LinearAlgebraBase& get(const std::string& name = "generic");
    static void regist(const std::string& name, LinearAlgebraBase*);

private:
    LinearAlgebraFactory();

    static LinearAlgebraFactory& instance();
    typedef std::map<std::string, LinearAlgebraBase*> FactoryMap;
    FactoryMap map_;
};

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
