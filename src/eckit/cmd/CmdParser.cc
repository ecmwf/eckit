/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <cstring>
#include <iomanip>
#include <stack>

#include "eckit/cmd/CmdParser.h"
#include "eckit/cmd/CmdResource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"
#include "eckit/utils/Tokenizer.h"

#define YY_INPUT(buf, result, max_size)                  \
    {                                                    \
        int c  = CmdParser::input();                     \
        result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
    }

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct EventNotFound : public Exception {
    EventNotFound(const std::string& s) :
        Exception(s + ": Event not found") {}
};

typedef std::stack<CmdParser*, std::vector<CmdParser*> > ParserStack;
typedef std::stack<CmdArg, std::vector<CmdArg> > CmdArgStack;

static std::ostream* out_ = &std::cout;

// On command line
static CmdArg arg_;
static CmdArg variables_;
static long param_;
static std::string command_;
static std::list<int> buffer_;
static bool prompt_  = true;
static bool console_ = false;

typedef std::map<char, bool, std::less<char> > FlagMap;
static FlagMap flags_;

// Alias are shared for every parser
static CmdArg theAlias_;
static CmdArg environment_;

// The history
typedef std::vector<std::string> History;
static History history_;

// For Lex
static bool eckit_cmd_debug_;

static long pos_ = 0;

//----------------------------------------------------------------------------------------------------------------------

namespace CmdYacc {

void eckit_cmd_error(const char* msg);

#include "eckit/cmd/cmdsy.c"
int eckit_cmd_wrap(void) {
    return 1;
}

void eckit_cmd_error(const char* msg) {
    Log::error() << msg << " line " << eckit_cmd_lineno << std::endl;
    if (flags_['e']) {
        Main::instance().terminate();
    }
}

#undef input
#undef unput
#undef output

}  // namespace CmdYacc

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::parse(const std::string& line, std::ostream& out) {
    command_ = line;
    pos_     = 0;
    out_     = &out;

    if (command_.size() > 0) {
        // Prepare for parse
        try {
            repeat();
            substitute();
            unAlias();

            buffer_.resize(command_.size());
            std::copy(command_.begin(), command_.end(), buffer_.begin());
            buffer_.push_back(0);

            // std::cout << command_ << std::endl;

            CmdYacc::eckit_cmd_parse();
        }
        catch (EventNotFound& e) {
            (*out_) << e.what() << std::endl;
        }
        catch (std::exception& e) {
            throw;
        }
    }
    reset();
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::parse(std::istream& in, std::ostream& out, const Prompter& prompter) {
    char l[102400];
    CmdYacc::eckit_cmd_debug = eckit_cmd_debug_;

    out_ = &out;
    in.tie(out_);

    prompt(prompter);
    reset();

    while (true) {
        if (console_) {
            const char* p = UserInput::getUserInput(prompter.prompt().c_str(), &CmdResource::completion);
            if (!p) {
                break;
            }
            strncpy(l, p, sizeof(l));
        }
        else {
            if (!in.getline(l, sizeof(l))) {
                break;
            }
        }
        char* p = l;
        while (p && *p == ' ')
            ++p;

        command_ = p;
        pos_     = 0;

        if (command_.size() > 0) {
            // Prepare for parse
            try {
                repeat();
                substitute();
                unAlias();

                // Print command if different from input
                if (command_ != l)
                    (*out_) << command_ << std::endl;

                buffer_.resize(command_.size());
                std::copy(command_.begin(), command_.end(), buffer_.begin());
                buffer_.push_back(0);

                if (prompt_)
                    historize();

                CmdYacc::eckit_cmd_parse();
            }
            catch (EventNotFound& e) {
                (*out_) << e.what() << std::endl;
            }
            catch (std::exception& e) {
                throw;
            }
        }

        prompt(prompter);
        reset();
    }
}

#if 0
void CmdParser::eckit_cmd_error(char* msg)
{
    Log::error() << msg << " line " << eckit_cmd_lineno << std::endl;
	if(flags_['e'])
	{
		Application::terminate();
	}
}
#endif

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::prompt(const Prompter& prompter) {

    if (prompt_)
        (*out_) << prompter.prompt() << "%" << history_.size() + 1 << "> " << std::flush;

    Log::status() << "Idle..." << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::historize() {
    Tokenizer tokenize(" \t\n");
    std::vector<std::string> tokens;
    tokenize(command_, tokens);

    if (tokens.size() > 0) {
        history_.push_back(command_);
        Log::debug() << "History: " << command_ << std::endl;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::shell(const std::string& s) {
    int ret = 0;
    if (not s.empty())
        ret = ::system(s.c_str());
    else
        ret = ::system("$SHELL");
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::history(const long n, std::ostream& out) {
    long size = history_.size();
    long from = n ? size - n : 0;

    if (from < 0)
        from = 0;
    for (int i = from; i < size; i++)
        out << std::setw(4) << i + 1 << "  " << history_[i] << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::environment(std::ostream& out) {
    out << environment_;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::environment(const std::string& lhs, const std::string& rhs) {
    environment_[lhs] = rhs;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::unAlias() {
    Tokenizer tokenize(" \t\n");
    std::vector<std::string> tokens;
    tokenize(command_, tokens);

    if (theAlias_.exists(tokens[0])) {
        std::string aliased = theAlias_[tokens[0]];

        Log::debug() << "Expanding '" << tokens[0] << "' to '" << aliased << std::endl;

        for (size_t i = 1; i < tokens.size(); ++i)
            aliased += " " + tokens[i];

        command_ = aliased;

        Log::debug() << "New command '" << command_ << "'" << std::endl;
        unAlias();
    }
}

//----------------------------------------------------------------------------------------------------------------------

int hIndex(const std::string& cmd, std::vector<std::string>& h, std::string& pre, std::string& post) {
    std::string::size_type i = cmd.find('!');
    if (i != std::string::npos) {
        const char* p = (cmd.c_str() + i + 1);  // Skip '!'
        int size      = h.size();

        pre = cmd.substr(0, i);
        Log::debug() << "In repeat() command" << std::endl;

        if (!p)
            return size;

        if (*p == '!') {
            post = ++p;
            return size - 1;
        }

        if ((*p == '-') || (isdigit((int)(*p)))) {
            int idx   = atoi(p);
            int start = (idx <= 0) ? size : -1;

            if (*p == '-')
                ++p;
            while (isdigit((int)(*p)))
                ++p;

            post = p;
            return start + idx;
        }

        for (int i = size - 1; i >= 0; --i)
            if (h[i].find(p, 0) == 0)
                return i;

        return size;
    }

    return -1;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::repeat() {
    Tokenizer tokenize(" \t\n");
    std::vector<std::string> tokens;
    tokenize(command_, tokens);

    if (!tokens.size())
        return;

    std::string pre(""), post("");
    int which = hIndex(tokens[0], history_, pre, post);

    if (which >= 0) {
        if (static_cast<size_t>(which) < history_.size()) {
            std::string newcmd = pre + history_[which];
            newcmd += post;
            for (size_t i = 1; i < tokens.size(); ++i)
                newcmd += " " + tokens[i];

            command_ = newcmd;
            Log::debug() << "New command '" << command_ << "'" << std::endl;
        }
        else {
            throw EventNotFound(tokens[0]);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::substitute() {
    if (command_[0] == '^') {
        Tokenizer tokenize("^");
        std::vector<std::string> tokens;
        tokenize(command_, tokens);

        if (tokens.size() == 2) {
            std::string lastcmd = history_[history_.size() - 1];
            int i               = lastcmd.find(tokens[0]);

            command_ = lastcmd.substr(0, i) + tokens[1] + lastcmd.substr(i + tokens[0].size());
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::run(void (*proc)(CmdResource*, CmdArg&, std::istream&, std::ostream&)) {
    Log::debug() << "Executing " << std::endl
                 << arg_ << std::endl;

    if (flags_['x'])
        Log::info() << command_ << std::flush;

    if (!flags_['n'])
        if (!CmdResource::run(proc, arg_, std::cin, *out_))
            if (flags_['e'])
                Main::instance().terminate();
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::alias(const std::string& name, const std::string& cmd) {
    theAlias_[name] = cmd;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::alias(const std::string& name) {
    (*out_) << theAlias_[name] << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::alias() {
    (*out_) << theAlias_ << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::aliasCompletion(const std::string& text, std::vector<std::string>& result) {
    std::vector<std::string> aliases = theAlias_.args();
    for (size_t i = 0; i < aliases.size(); i++)
        if (aliases[i].find(text) == 0)
            result.push_back(aliases[i]);
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::function(const std::string& lines) {}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::flag(const char flag, bool value) {
    Log::debug() << "Setting flag " << flag << " to " << value << std::endl;
    flags_[flag] = value;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::flags(const std::string& s) {
    std::string flags = "exntuv";

    for (size_t j = 0; j < flags.length(); ++j)
        flags_[flags[j]] = false;

    Tokenizer tokenize(" ");
    std::vector<std::string> tokens;
    tokenize(s, tokens);

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].length() == 2) {
            char begin = tokens[i][0];
            if (begin == '-')
                flags_[tokens[i][1]] = true;
            if (begin == '+')
                flags_[tokens[i][1]] = false;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::reset() {
    param_ = 0;
    arg_.erase();
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::addCmd(const char* s) {
    command_ += s;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::addCmd(const char c) {
    command_ += c;
}

//----------------------------------------------------------------------------------------------------------------------

int CmdParser::input() {
    if (buffer_.empty())
        return ';';

    int n = buffer_.front();
    buffer_.erase(buffer_.begin());
    return n;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::unput(int n) {
    buffer_.push_front(n);
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::output(int n) {
    (*out_) << char(n);
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::arg(long n, const Value& v) {
    arg_[n] = v;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::arg(const std::string& s, const Value& v) {
    arg_[s] = v;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::arg(const Value& v) {
    arg_[++param_] = v;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::var(const std::string& s) {
    arg_[++param_] = variables_.exists(s) ? std::string(variables_[s]) : std::string(environment_[s]);
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::var(const std::string& s, const Value& v) {
    variables_[s] = v;
}

//----------------------------------------------------------------------------------------------------------------------

void CmdParser::prompt(bool p) {
    prompt_ = p;
}

void CmdParser::console(bool c) {
    console_ = c;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
