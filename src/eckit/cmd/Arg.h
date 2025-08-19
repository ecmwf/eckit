/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @author Manuel Fuentes
/// @date   Jan 1997

#ifndef eckit_cmd_Arg_H
#define eckit_cmd_Arg_H

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>


namespace eckit {

//-----------------------------------------------------------------------------

class ArgContent;

class Arg {
public:

    enum Type {
        number,
        text,
        path,
        tape,
        file,
        expression,
        boolean,
        ellipsis
    };

    // -- Contructors

    Arg();
    Arg(const std::string& option, Type = boolean);

    // -- Destructor

    ~Arg();

    // -- Copy

    Arg(const Arg&);
    Arg& operator=(const Arg&);

    // -- Operators

    Arg operator~();

    // -- Methods
    std::vector<std::string> completion(std::vector<std::string>&);

protected:

    // -- Methods

    void print(std::ostream&) const;

private:

    Arg(ArgContent*);

    // -- Members

    std::unique_ptr<ArgContent> content_;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Arg& p) {
        p.print(s);
        return s;
    }

    friend Arg operator|(const Arg&, const Arg&);
    //  friend Arg operator,(const Arg&,const Arg&);
    friend Arg operator+(const Arg&, const Arg&);
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
