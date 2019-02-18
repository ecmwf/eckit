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

#include <unistd.h>

#include "eckit/eckit.h"

#ifdef YYBISON
#define YYSTYPE_IS_DECLARED
int yylex();
/* int yydebug;*/
extern "C" int isatty(int);
#endif

struct YYSTYPE {
    std::string str;
    long long num;  // change to long long, and in lex as well..
    Value val;
};

inline Value Function(const std::string& op, const Value& a) {
    return Value::makeList(Value(op)) + Value::makeList(a);
}

inline Value Function(const std::string& op, const Value& a, const Value& b) {
    return Value::makeList(Value(op)) + Value::makeList(a) + Value::makeList(b);
}

%}

%token <str>STRING
%token <str>OPTION
%token <str>SHELL
%token <str>PIPE
%token <str>OUTPUT
%token <str>OUTAPPEND
%token <num>NUMB

%type  <val>value
%type  <val>list

%type  <val>expression
%type  <val>condition
%type  <val>conjonction
%type  <val>term
%type  <val>factor
%type  <val>power
%type  <val>disjonction

%type  <num>ages
%type  <num>age
%type  <num>sizes
%type  <num>size
%type  <num>number

%token DAY
%token MONTH
%token WEEK
%token HOUR
%token MINUTE
%token SECOND
%token YEAR

%token AND
%token OR
%token LE
%token GE
%token NE
%token NOT
%token IN
%token MATCH

%token HEXABYTE
%token PETABYTE
%token TERABYTE
%token GIGABYTE
%token MEGABYTE
%token KILOBYTE
%token BYTE

%%

start : lines
	  | empty
	  ;

lines : lines line
      | line
	  ;

line  : command eol
      | eol
	  | error eol
	  ;

eol   : ';'						{ CmdParser::reset(); }
	  | empty
	  ;

command : STRING args           {
								  CmdParser::arg(0,$1);
								  CmdParser::run(CmdResource::command);
								}
        | '@' STRING args       {
								  CmdParser::arg(0,$2);
								  CmdParser::run(CmdResource::loop);
								}
        | STRING args PIPE      {
								  CmdParser::arg("|",$3);
								  CmdParser::arg(0,$1);
								  CmdParser::run(CmdResource::pipe);
								}
        | STRING args OUTPUT    {
								  CmdParser::arg(">",$3);
								  CmdParser::arg(0,$1);
								  CmdParser::run(CmdResource::redirect);
								}
        | STRING args OUTAPPEND {
								  CmdParser::arg(">>",$3);
								  CmdParser::arg(0,$1);
								  CmdParser::run(CmdResource::append);
								}
		| STRING '=' value       { CmdParser::var($1,$3); }
		| SHELL                  { CmdParser::shell($1);    }
		;

args   : empty
	   | arg args
	   | arg
	   | args params
	   | variable
	   ;

arg   : OPTION value    { CmdParser::arg($1,$2); }
      | OPTION          { CmdParser::arg($1,std::string("1")); }
	  ;

variable : '$' STRING   { CmdParser::var($2); }
		 ;

params : params param
       | param
       | empty
	   ;

param : value {  CmdParser::arg($1); }
	  ;

value : '[' list ']'       { $$ = $2; }
	  | '[' ']'            { $$ = Value::makeList(); }
	  | number             { $$ = Value($1); }
	  | '-' number         { $$ = Function("neg",$2); }
	  | STRING             { $$ = Value($1); }
	  | '(' expression ')' { $$ = $2; }
	  | STRING '(' ')'     { $$ = Value($1 + "()"); }
	  ;



power  		: value '^' power     { $$ = Function("pow",$1,$3); }
	   		| value
	   		;

factor 		: factor '*' power   { $$ = Function("mul",$1,$3); }
	   		| factor '/' power   { $$ = Function("div",$1,$3); }
	   		| factor '%' power   { $$ = Function("mod",$1,$3); }
	   		| power
	   		;

term   		: term '+' factor    { $$ = Function("add",$1,$3); }
	   		| term '-' factor    { $$ = Function("sub",$1,$3); }
	   		| term '&' factor    { $$ = Function("cat",$1,$3); }
	  	 	| factor
	  		;

condition 	: condition '>'     term { $$ = Function("gt",$1,$3); }
	 		| condition '='     term { $$ = Function("eq",$1,$3); }
	 		| condition '<'     term { $$ = Function("lt",$1,$3); }
	 		| condition  GE     term { $$ = Function("ge",$1,$3); }
	 		| condition  LE     term { $$ = Function("le",$1,$3); }
	 		| condition  NE     term { $$ = Function("ne",$1,$3); }
	 		| condition  IN     term { $$ = Function("in",$1,$3); }
	 		| condition  MATCH  term { $$ = Function("match",$1,$3); }
	 		| NOT condition          { $$ = Function("not",$2); }
			| term
	 		;

conjonction : conjonction AND condition { $$ = Function("and",$1,$3); }
			| condition
			;

disjonction	: disjonction OR conjonction { $$ = Function("or",$1,$3); }
			| conjonction
			;

expression 	: disjonction
			;


list : list ',' value    { $$ = $1 + Value::makeList($3); }
	 | value             { $$ = Value::makeList($1); }
	 ;

number    :  ages
          |  sizes
          |  NUMB
		  ;

ages      : ages age { $$ = $1 + $2; }
          | age
		  ;

age       : NUMB SECOND { $$ = $1;}
		  | NUMB MINUTE { $$ = $1 * 60;}
		  | NUMB HOUR   { $$ = $1 * 60 * 60;}
		  | NUMB DAY    { $$ = $1 * 60 * 60 * 24;}
		  | NUMB WEEK   { $$ = $1 * 60 * 60 * 24 * 7;}
		  | NUMB MONTH  { $$ = $1 * 60 * 60 * 24 * 31;}
		  | NUMB YEAR   { $$ = $1 * 60 * 60 * 24 * 365;}
		  ;

sizes     : sizes size { $$ = $1 + $2; }
		  | size
		  ;

size      : NUMB BYTE      { $$ = $1;}
		  | NUMB KILOBYTE  { $$ = $1 * 1024; }
		  | NUMB MEGABYTE  { $$ = $1 * 1024 * 1024; }
		  | NUMB GIGABYTE  { $$ = $1 * 1024 * 1024 * 1024; }
		  | NUMB TERABYTE  { $$ = $1 * 1024 * 1024 * 1024 * 1024; }
		  | NUMB PETABYTE  { $$ = $1 * 1024 * 1024 * 1024 * 1024 * 1024; }
		  | NUMB HEXABYTE  { $$ = $1 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024; }
		  ;

empty :
      ;

%%
#undef YY_NULL
#include "cmdsl.c"
