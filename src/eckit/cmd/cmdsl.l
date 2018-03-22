%{

/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#undef YYLMAX
#define YYLMAX 2048
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eckit/cmd/CmdParser.h"

static int paren = 0; // Not thread safe, but are lex and yacc safe anyway?

#ifdef __linux__
#define YY_INPUT(buf,result,max_size) { int c = CmdParser::input(); result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); }
#endif


#undef input
#define input()  CmdParser::input()

#undef unput
#define unput(n) CmdParser::unput(n)

#undef output
#define output(n) CmdParser::output(n)

%}

IDENT   [_A-Za-z/:]+[_0-9A-Za-z/:\.]*
WORD    [A-Za-z:]+[_0-9A-Za-z/:\.]*
NUMB    [0-9]+
DIG1    [0-9]

%s EXPRESSION

%%

<INITIAL>\|[a-z A-Z0-9/_$:\-\.\'\|\"]*  {
					yylval.str = (const char *)yytext+1;
					return PIPE;
				}

<INITIAL>\%[a-z A-Z0-9/_$:\-\.\'\|\"]*  {
					yylval.str = (const char *)yytext+1;
					return SHELL;
				}

<INITIAL>>[a-z A-Z0-9/_$:\-\.\'\|\"]*  {
					yylval.str = (const char *)yytext+1;
					return OUTPUT;
				}


<INITIAL>"("    { paren=1 ; BEGIN EXPRESSION ;     return *yytext ;}
<EXPRESSION>"(" { paren++ ;                        return *yytext ;}
<EXPRESSION>")" { if(--paren == 0) BEGIN INITIAL ; return *yytext ;}
<EXPRESSION>">=" { return GE ; }
<EXPRESSION>">" { return *yytext ; }
<EXPRESSION>"/" { return *yytext ; }

\"|\'  	{
           int c,q = yytext[0];

           yyleng = 0;

           while((c = input()) && c != q && c != '\n')
           {
               if(c == '\\') yytext[yyleng++] = input();
               else yytext[yyleng++] =  c;
            }

            yytext[yyleng++] = 0;
            yylval.str = (const char *)yytext;
            return STRING;
        }

"and"        { return AND; }
"&&"         { return AND; }
"=="         { return '='; }
"!="         { return NE; }
">="         { return GE; }
"<="         { return LE; }
"<>"         { return NE; }
"or"         { return OR; }
"||"         { return OR; }
"not"        { return NOT; }
"!"          { return NOT; }

"=~"    	 { return MATCH; }
"match"      { return MATCH; }

"day"        { return DAY; }
"days"       { return DAY; }
"minute"     { return MINUTE; }
"minutes"    { return MINUTE; }
"second"     { return SECOND; }
"seconds"    { return SECOND; }
"week"       { return WEEK; }
"weeks"      { return WEEK; }
"month"      { return MONTH; }
"months"     { return MONTH; }
"year"       { return YEAR; }
"years"      { return YEAR; }
"hour"       { return HOUR; }
"hours"      { return HOUR; }

"Hb"         { return HEXABYTE; }
"Pb"         { return PETABYTE; }
"Tb"         { return TERABYTE; }
"Gb"         { return GIGABYTE; }
"Mb"         { return MEGABYTE; }
"Kb"         { return KILOBYTE; }
"byte"       { return BYTE; }
"bytes"      { return BYTE; }

<INITIAL>"-"{IDENT}	  { yylval.str = (const char *)yytext+1; return OPTION; }
<INITIAL>{IDENT} 	  { yylval.str = (const char *)yytext; return STRING; }
<EXPRESSION>{WORD}	  { yylval.str = (const char *)yytext; return STRING; }
{NUMB} 		          { yylval.num = Translator<std::string,long long>()((const char *)yytext); return NUMB; }
\#      {
           int c;
           while((c = input()) && (c != '\n'))
                    ;
		   return '\n';
        }
[ \t]*  ;
\n      { return *yytext; }

.	{ return *yytext; }
%%
    /* "loop"       { return LOOP; } */
    /* "end"        { return END; } */
