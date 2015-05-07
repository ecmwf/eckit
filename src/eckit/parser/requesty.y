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

#include <unistd.h>

#ifdef YYBISON
#define YYSTYPE_IS_DECLARED
int yylex();
/*int yydebug;*/
#endif

typedef std::pair<std::string,std::vector<std::string> > Par;
typedef std::list<Request>                               ReqList;
typedef std::list<Par>                                   ParList;

struct YYSTYPE {
    std::string               str;
    ReqList                   req_list;
    Request                   req;
    std::vector<std::string>  vec;
    Par                       par;
    ParList                   par_list;
};

static void update(Request& r, const ParList& p)
{
	for(ParList::const_iterator j = p.begin(); j != p.end(); ++j)
		r[(*j).first] = (*j).second;
}

static void name(Request& r, const std::string& s)
{
    std::vector<std::string> ps;
    ps.push_back(s);
    r["_verb"] = ps; 
}

%}

%type <req_list>requests;

%type <req>request
%type <str>value
%type <vec>values
%type <par>parameter
%type <par_list>parameters

%token <str>WORD
%token <str>STRING
%token <str>NUMB

%start all

%%

all:	 requests  { RequestParserResult::result_ = $1; }
	;

requests : request            { $$ = ReqList(); $$.push_back($1); }
		 | requests request   { $$ = $1;        $$.push_back($2); }
		 ;

request : WORD                     { $$ = Request(); name($$, $1); }
		| WORD ',' parameters      { $$ = Request(); name($$, $1); update($$, $3); }
		| WORD '.'                 { $$ = Request(); name($$, $1); }
		| WORD ',' parameters '.'  { $$ = Request(); name($$, $1); update($$, $3); }
		;

parameters : parameter                { $$ = ParList(); $$.push_back($1);  }
		   | parameters ',' parameter { $$ = $1;        $$.push_back($3);  }
		   ;

parameter : WORD '=' values        { $$ = std::pair<std::string,std::vector<std::string> >($1, $3); }
		  ;

values : value             { $$ = std::vector<std::string>(); $$.push_back($1);  }
	   | values '/' value  { $$ = $1;               $$.push_back($3);  }
	   ;

value: WORD   
	 | STRING 
	 | NUMB
	 ;

%%
#include "requestl.c"

