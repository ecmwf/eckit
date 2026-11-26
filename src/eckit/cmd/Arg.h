// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
