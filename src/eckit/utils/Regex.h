// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Regex.h
// Baudouin Raoult - ECMWF Jan 98

#ifndef eckit_Regex_h
#define eckit_Regex_h

#include <regex.h>

#include <string>
#include <string_view>


namespace eckit {

//--------------------------------------------------------------------------------------------------

class Regex {
public:

    // -- Contructors

    Regex(const std::string& = ".*", bool shell = false, bool extended = true);
    Regex(const Regex&);

    ~Regex();

    // -- Methods

    Regex& operator=(const Regex&);

    bool match(const std::string& s) const;

    operator const std::string&() const { return str_; }

    bool operator==(const Regex& other) const { return str_ == other.str_; }

    static std::string escape(std::string_view);

protected:  // methods

    void print(std::ostream&) const;

private:  // members

    std::string str_;
    regex_t re_;
    bool extended_;

private:  // methods

    void compile(const char*);

    friend std::ostream& operator<<(std::ostream& s, const Regex& p) {
        p.print(s);
        return s;
    }
};


//--------------------------------------------------------------------------------------------------


}  // namespace eckit

#endif
