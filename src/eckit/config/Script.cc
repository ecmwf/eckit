/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"

#include "eckit/config/Script.h"
#include "eckit/config/Block.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace config {

//-----------------------------------------------------------------------------

Script::Script()
{
}

Script::Script( Compiler& c )
{
    Block* blk = new Block(c, new Scope() );

    ASSERT(c.peek() == 0);

    blocks_.push_back(blk);
}

Script::~Script()
{
    clear();
}

void Script::execute(const StringDict &in, StringDict &out)
{
    for( BlockStore::iterator i = blocks_.begin(); i != blocks_.end(); ++i )
        (*i)->execute(in,out);
}

void Script::print(std::ostream&out)
{
    for( BlockStore::iterator i = blocks_.begin(); i != blocks_.end(); ++i )
        (*i)->print(out);
}

void Script::readStream(std::istream &in)
{
    Compiler c(in);

    Block* blk = new Block(c, new Scope() );

    ASSERT(c.peek() == 0);

    blocks_.push_back(blk);
}

void Script::clear()
{
    for( BlockStore::iterator i = blocks_.begin(); i != blocks_.end(); ++i )
        delete (*i);
}

//-----------------------------------------------------------------------------

} // namespace config
} // namespace eckit
