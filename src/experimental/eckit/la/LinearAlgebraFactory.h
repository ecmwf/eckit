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

#include "eckit/config/Configurable.h"
#include "eckit/config/Resource.h"

#include "experimental/eckit/la/LinearAlgebraBase.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

class LinearAlgebraFactory : public Configurable {

public:
    static const LinearAlgebraBase* get();
    static const LinearAlgebraBase* get(const std::string& name);
    static void regist(const std::string& name, const LinearAlgebraBase*);
    static void list(std::ostream &);

    // -- from Configurable
    virtual std::string kind() const { return "LinearAlgebraFactory"; }
    virtual std::string name() const { return "eckit"; }
    virtual void reconfigure();

private:
    LinearAlgebraFactory();

    static LinearAlgebraFactory& instance();

    typedef std::map<std::string, const LinearAlgebraBase*> FactoryMap;
    FactoryMap map_;
    Resource<std::string> backend_;
};

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
