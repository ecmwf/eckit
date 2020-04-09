/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iomanip>

#include "eckit/cmd/CmdArg.h"
#include "eckit/cmd/CmdParser.h"
#include "eckit/cmd/CmdResource.h"
#include "eckit/cmd/TermBuf.h"

#include "eckit/config/Resource.h"
#include "eckit/io/StdPipe.h"
#include "eckit/io/StdioBuf.h"
#include "eckit/log/Log.h"
#include "eckit/os/SignalHandler.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/utils/Tokenizer.h"

// TODO: remember to add a mutex


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CmdResource::Map* CmdResource::resources_ = nullptr;

CmdResource::CmdResource(const std::string& s) {
    Tokenizer tokenize(",");
    std::vector<std::string> tokens;
    tokenize(s, tokens);

    if (!resources_)
        resources_ = new Map();
    for (size_t i = 0; i < tokens.size(); i++)
        (*resources_)[tokens[i]] = this;
}

CmdResource::~CmdResource() {
    // Should do something here...
}

void CmdResource::loop(CmdResource* cmd, CmdArg& arg, std::istream& in, std::ostream& o) {
    long s = eckit::Resource<long>("loopDelay", 2);
    TermBuf buf(o);

    struct Hide {
        Hide() { Monitor::instance().show(false); }
        ~Hide() { Monitor::instance().show(true); }
    };

    //	Hide dontshow;

    buf.init();
    buf.clear();
    std::ostream out(&buf);

    // SignalHandler interrupt;
    // std::string strcmd(arg[0]);

    for (;;) {
        // Log::status() << "Executing '" << strcmd << "'" << std::endl;
        buf.home();
        cmd->execute(in, out, arg);
        buf.clearEOS();
        ::sleep(s);
        SignalHandler::checkInterrupt();
    }
}

void CmdResource::command(CmdResource* cmd, CmdArg& arg, std::istream& in, std::ostream& out) {
    cmd->execute(in, out, arg);
    out << std::flush;
}

void CmdResource::print(std::ostream& out) const {
    Map* m = resources_;

    out << "Contents: " << std::endl;
    for (Map::iterator i = m->begin(); i != m->end(); ++i)
        out << (*i).first << " = " << (*i).second << std::endl;
}

std::vector<std::string> CmdResource::completion(const std::string& c) {
    Map* m = resources_;
    std::vector<std::string> result;

    for (Map::iterator i = m->begin(); i != m->end(); ++i) {
        std::string cmd = (*i).first;
        if (cmd.find(c) == 0) {
            result.push_back(cmd);
        }
    }

    CmdParser::aliasCompletion(c, result);

    std::sort(result.begin(), result.end());

    return result;
}

std::vector<std::string> CmdResource::completion(const std::vector<std::string>& r) {
    std::vector<std::string> result;
    std::vector<std::string> copy(r);

    Map* m          = resources_;
    Map::iterator j = m->find(r[0]);

    if (j != m->end()) {
        Arg a((*j).second->usage(r[0]));
        result = a.completion(copy);
    }

    return result;
}

// Called by UserInput
bool CmdResource::completion(const char* line, int pos, char* insert, int insertmax) {

    std::vector<std::string> c;
    const char* p = line;
    int n         = 0;

    c.push_back("");
    while (true) {

        if (n == pos) {
            std::vector<std::string> v;
            if (c.size() == 1) {
                v = completion(c[0]);
            }
            else {
                v = completion(c);
            }


            if (v.size() == 1) {
                for (size_t i = c.back().length(); i < v[0].length() && i < size_t(insertmax); i++) {
                    *insert++ = v[0][i];
                }
                *insert = 0;
                return true;
            }

            // Copy matches

            int k = 0;
            for (size_t i = 0; i < v.size(); i++) {

                if (i && k < insertmax) {
                    *insert++ = ' ';
                    k++;
                }

                for (size_t j = 0; j < v[i].length() && k < insertmax; j++, k++) {
                    *insert++ = v[i][j];
                }
                *insert = 0;
            }
            return false;
        }

        if (*p == 0) {
            break;
        }

        if (*p == ' ') {
            c.push_back("");
        }
        else {
            c.back().push_back(*p);
        }
        p++;
        n++;
    }

    return false;  // silence compiler warning
}

void CmdResource::help(std::ostream& out, const std::string& cmdname) {
    Map* m = resources_;

    if (cmdname == "") {
        for (Map::iterator i = m->begin(); i != m->end(); ++i) {
            std::string cmd = (*i).first;
            out << " " << cmd << std::setw(16 - cmd.length()) << std::setfill(' ') << " ";
            //(*i).second->help(out);
            out << " " << (*i).second->usage(cmd);
            out << std::endl;
        }
    }
    else {
        Map::iterator j = m->find(cmdname);

        if (j != m->end()) {
            CmdResource* cmd = (*j).second;
            out << cmdname;
            out << std::setw(15 - cmdname.length()) << std::setfill(' ') << " ";
            out << cmd->usage(cmdname) << std::endl;
            out << std::endl;
            cmd->help(out);
            out << std::endl;
        }
        else
            out << " '" << cmdname << "' not found" << std::endl;
    }
}

bool CmdResource::run(void (*proc)(CmdResource*, CmdArg&, std::istream&, std::ostream&), CmdArg& arg, std::istream& in,
                      std::ostream& out) {
    static bool fail = eckit::Resource<bool>("-fail", false);

    const std::string strcmd = arg[0];

    Map* m          = resources_;
    Map::iterator j = m->find(strcmd);

    if (j != m->end()) {
        CmdResource* cmd = (*j).second;
        Log::status() << "Executing '" << strcmd << "'" << std::endl;
        try {
            SignalHandler interrupt;
            proc(cmd, arg, in, out);
            return true;
        }
        catch (Abort& e) {
            if (fail)
                throw;
        }
        catch (std::exception& e) {
            if (fail)
                throw;

            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
    }
    else {
        if (fail)
            throw eckit::SeriousBug(strcmd + ": command not found");
        out << "'" << strcmd << "': command not found" << std::endl;
    }

    return false;
}

void CmdResource::redirect(CmdResource* cmd, CmdArg& args, std::istream& in, std::ostream&) {
    Tokenizer tokenize(" ");
    std::vector<std::string> tokens;
    tokenize(std::string(args[">"]), tokens);

    std::string file = tokens[0];
    std::ofstream out(file.c_str());

    if (!out)
        throw CantOpenFile(file);

    CmdArg newargs = args;
    newargs.erase(">");

    cmd->execute(in, out, newargs);

    out.close();
    if (out.bad())
        throw WriteError(file);
}

void CmdResource::append(CmdResource* cmd, CmdArg& args, std::istream& in, std::ostream&) {
    Tokenizer tokenize(" ");
    std::vector<std::string> tokens;
    tokenize(std::string(args[">>"]), tokens);

    std::string file = tokens[0];
    std::ofstream out(file.c_str(), std::ios::app);
    if (!out)
        throw CantOpenFile(file);

    CmdArg newargs = args;
    newargs.erase(">>");

    cmd->execute(in, out, newargs);

    out.close();
    if (out.bad())
        throw WriteError(file);
}

void CmdResource::pipe(CmdResource* cmd, CmdArg& args, std::istream& in, std::ostream&) {

    const std::string to = args["|"];  // everything after the pipe

    StdPipe pipe(to, "w");
    AutoCloser<StdPipe> closer(pipe);
    StdioBuf buf(pipe);
    std::ostream out(&buf);

    CmdArg newargs = args;
    newargs.erase("|");

    cmd->execute(in, out, newargs);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
