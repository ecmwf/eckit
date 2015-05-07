%{

/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#undef YYLMAX
#define YYLMAX 2048
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eckit/parser/RequestParser.h"

%}

IDENT   [_0-9A-Za-z]+[_\.\-\+A-Za-z0-9:\t ]*[_\.\-\+A-Za-z0-9]+
NUMB    [\-\.]*[0-9]+[\.0-9]*[Ee]*[\-\+]*[0-9]*

%%

{IDENT}      { yylval.str = (const char *)yytext; return WORD; }
{NUMB}       { yylval.str = (const char *)yytext; return WORD; }

\"|\'  {
           int c,q = yytext[0];
           yyleng = 0;

           while((c = yyinput()) && c != q)
           {
               if(c == '\\') yytext[yyleng++] = yyinput();
               else yytext[yyleng++] =  c;
            }

            yytext[yyleng++] = 0;
            yylval.str = (const char *)yytext;
            return STRING;
        }

\#      {
            int c;
            while((c = yyinput()) && (c != '\n'))
                    ;
        }

[ \t]*  ;
\n      ; // TODO: FIXME: lineno++;

.              return *yytext;

%%
