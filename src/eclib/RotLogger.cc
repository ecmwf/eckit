/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Application.h"
#include "eclib/PathName.h"
#include "eclib/Resource.h"
#include "eclib/RotLogger.h"
#include "eclib/TimeStamp.h"

static ofstream* last     = 0;
static time_t   lastTime  = 0;

RotLogger::RotLogger() : DHSLogger()
{
}

RotLogger::~RotLogger()
{
}

std::ostream& RotLogger::out()
{
    time_t now = ::time(0) / 86400;
    if(now != lastTime || last == 0)
    {
        static string logfileFormat = Resource<string>("logfileFormat","~/log/%Y-%m-%d/out");

        TimeStamp ts(logfileFormat);
        PathName path(ts);
        path.mkdir(0777);

        StrStream os;
        os << path  << "/" << Application::appName() << StrStream::ends;

        delete last;

        // TODO: Find a way to set the close on exec flags
        last = new ofstream(string(os).c_str(),ios::app);

        lastTime = now;

    }

    return *last;
}
