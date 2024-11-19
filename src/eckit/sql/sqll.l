%option reentrant
%option extra-type="void *"
%option bison-bridge
%option yylineno
%{

#define YY_USER_ACTION do { \
        struct yyguts_t * yyg ((struct yyguts_t*) yyscanner); \
        do { (void)(yyg); } while( 0 ); \
        /* fprintf(stderr, "location <%d,%d>,<%d,%d>\n", yyg->yylloc_r.first_line, yyg->yylloc_r.first_column, yyg->yylloc_r.last_line, yyg->yylloc_r.last_column); */ \
    } while (0);

/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

// http://flex.sourceforge.net/manual/Multiple-Input-Buffers.html#Multiple-Input-Buffers

class Stack {
    static const size_t maxIncludeDepth = 2000;
public:
    void push(const std::string& sql, const std::string& yypath, YY_BUFFER_STATE state, eckit_sql_scan_t scanner)
    {
        if (stack_.size() > maxIncludeDepth)
            throw eckit::UserError("Includes nested too deeply");

        stack_.push_back(state);
        currentBuffer_ = eckit_sql__scan_bytes(sql.c_str(), sql.size(), scanner);
        eckit_sql__switch_to_buffer(currentBuffer_, scanner);
    }

    int pop(eckit_sql_scan_t scanner)
    {
        if (stack_.size() <= 1)
            return 1; 

        YY_BUFFER_STATE currentState (stack_.back());
        stack_.pop_back();

        eckit_sql__delete_buffer(currentBuffer_, scanner);
        eckit_sql__switch_to_buffer(currentState, scanner);
        return 0;
    }

private:
    YY_BUFFER_STATE currentBuffer_;
    std::vector<YY_BUFFER_STATE> stack_;
};

typedef Stack include_stack;

/*
%option noyywrap
*/

%}
IDENT   [_A-Za-z]+[_0-9A-Za-z]*
VAR     [$][_0-9A-Za-z]+
NUMB    [-]?[0-9]+(\.[0-9]*)?
SEMICOLON [;]


%s LEX_ORDERBY
%s LEX_CREATE
%x incl

%%
#include                          BEGIN(incl);

@[lL][iI][nN][kK]                 return LINK;
[sS][eE][tT]                      return SET;
[dD][aA][tT][aA][bB][aA][sS][eE]  return DATABASE;
[sS][eE][lL][eE][cC][tT]          return SELECT;
[iI][nN][tT][oO]                  return INTO;
[fF][rR][oO][mM]                  return FROM;
[wW][hH][eE][rR][eE]              return WHERE;
[iI][nN][sS][eE][rR][tT]          return INSERT;
[vV][aA][lL][uU][eE][sS]          return VALUES;
[aA][nN][dD]                      return AND;
[oO][rR]                          return OR;
[oO][nN]                          return ON;
[iI][sS]                          return IS;
[nN][uU][lL][lL]                  return NIL;
[nN][oO][tT]                      return NOT;
[cC][oO][uU][nN][tT]              return COUNT;
[eE][xX][iI][tT]                  return EXIT;
[vV][iI][eE][wW]                  { BEGIN 0; return VIEW; }
[cC][rR][eE][aA][tT][eE]          { BEGIN LEX_CREATE; return CREATE; }
[sS][cC][hH][eE][mM][aA]          { BEGIN 0; return SCHEMA; }
<LEX_CREATE>[iI][nN][dD][eE][xX]  { BEGIN 0; return INDEX; }
<LEX_CREATE>[tT][aA][bB][lL][eE]  { BEGIN 0; return TABLE; }
<LEX_CREATE>[tT][yY][pP][eE]      { BEGIN 0; return TYPE; }
[tT][yY][pP][eE][dD][eE][fF]      { BEGIN 0; return TYPEDEF; }
[tT][yY][pP][eE][oO][fF]          return TYPEOF;
[bB][eE][tT][wW][eE][eE][nN]      return BETWEEN;
[dD][iI][sS][tT][iI][nN][cC][tT]  return DISTINCT;
[aA][lL][lL]                      return ALL;
[mM][iI][sS][sS][iI][nN][gG]      return NIL;
[gG][rR][oO][uU][pP]              return GROUP;
[oO][rR][dD][eE][rR]              { BEGIN LEX_ORDERBY; return ORDER; }
[bB][yY]                          return BY;
[iI][nN]                          return IN;
[rR][eE][aA][dD][oO][nN][lL][yY]  return READONLY;
[uU][pP][dD][aA][tT][eE][dD]      return UPDATED;
[nN][oO][rR][eE][oO][rR][dD][eE][rR] return NOREORDER;
[sS][aA][fF][eE][gG][uU][aA][rR][dD] return SAFEGUARD;
[tT][eE][mM][pP][oO][rR][aA][rR][yY] return TEMPORARY;
<LEX_ORDERBY>[aA][sS][cC]         return ASC;
<LEX_ORDERBY>[dD][eE][sS][cC]     return DESC;
{SEMICOLON}	                      { BEGIN 0; return ';'; }
[aA][sS]                          return AS;
\#                                return HASH; 
[cC][oO][nN][sS][tT][rR][aA][iI][nN][tT] return CONSTRAINT;
[uU][nN][iI][qQ][uU][eE]          return UNIQUE;
[pP][rR][iI][mM][aA][rR][yY]      return PRIMARY;
[fF][oO][rR][eE][iI][gG][nN]      return FOREIGN;
[kK][eE][yY]                      return KEY;
[rR][eE][fF][eE][rR][eE][nN][cC][eE][sS] return REFERENCES;
[dD][eE][fF][aA][uU][lL][tT]      return DEFAULT;
[iI][nN][hH][eE][rR][iI][tT][sS]  { BEGIN 0; return INHERITS; }
[mM][aA][tT][cC][hH]              return MATCH;
[qQ][uU][eE][rR][yY]              return QUERY;
[lL][iI][kK][eE]                  return LIKE;
[rR][lL][iI][kK][eE]              return RLIKE;
[aA][lL][iI][gG][nN]              return ALIGN;
[rR][eE][sS][eE][tT]              return RESET;
[dD][uU][aA][lL]                  return DUAL;
[oO][nN][eE][lL][oO][oO][pP][eE][rR] return ONELOOPER;
<incl>[ \t]*                      /* eat the whitespace */
<incl>[^ \t\n;]+ {                 /* name of the file to be included */
    //cerr << "include " << yytext << std::endl;

    eckit::PathName fileName(eckit::StringTools::unQuote(yytext));

    //cerr << "Opening " << fileName << std::endl;
    std::string text (SQLSession::readIncludeFile(fileName));

    Stack* includeStack (static_cast<Stack*>(((struct SQLYacc::eckit_sql_guts_t*) eckit_sql_scanner)->eckit_sql_extra_r));
    ASSERT(includeStack);
    includeStack->push(text, fileName, YY_CURRENT_BUFFER, eckit_sql_scanner);
    BEGIN(INITIAL);
}

<<EOF>> { 
    //cerr << "** EOF **" << std::endl;
    eckit_sql_terminate();
}

\"|\'   {
            int c, end = yytext[0];

            yyleng = 0;
            while ((c = yyinput(yyscanner)) && c != end)
            {
                yytext[yyleng++] = (c == '\\') ? yyinput(yyscanner) : c;
                if (c == '\n')
                {
                    struct yyguts_t * yyg = (struct yyguts_t*) yyscanner;
                    ++ yyg->yylineno_r;
                }
            }

            yytext[yyleng++] = 0;
            yylval->val = yytext;
            return STRING;
        }

\{

{IDENT}	{ 
			char *p = yytext; while(*p) { *p = tolower(*p); p++; }
			yylval->val = yytext; 
			//cerr << "****I " << yylval.val << " ****" << std::endl; 
			return IDENT; 
		}
{VAR}   { 
			char *p = yytext; while(*p) { *p = tolower(*p); p++; }
			yylval->val = yytext; 
			//cerr << "****V " << yylval.val << " ****" << std::endl; 
			return VAR; 
		}

{NUMB}	{
			yylval->num = atof(yytext);
			//cerr << "****N " << yylval.num << " ****" << std::endl; 
			return DOUBLE;
		}

"--".*  ;
"//".*  ;

[ \t]*  ;
\n      {
            struct yyguts_t * yyg = (struct yyguts_t*) yyscanner;
            ++ yyg->yylineno_r;
        }

"="       return EQ;
"=="      return EQ;
"!="      return NE;
"<>"      return NE;
">="      return GE;
"<="      return LE;
"!"       return NOT;

.		return *yytext;

%%
