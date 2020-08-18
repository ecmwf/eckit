%pure-parser
%lex-param {void * scanner}
%lex-param {eckit::sql::SQLSession* session}
%parse-param {void * scanner}
%parse-param {eckit::sql::SQLSession* session}

%{

/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define YYMAXDEPTH 500

using namespace SQLYacc;

typedef SQLExpression* SQLExpressionPtr; // For casts.


// n.b. Because all elements need to be valid, we cannot use references or reference_wrappers
//      --> we use raw pointers to items.
//      --> Convert to references on the output boundary

struct YYSTYPE {
    std::shared_ptr<SQLExpression>                 exp;
    SQLTable*                                      table;
    double                                         num;
    std::string                                    val;
    std::vector<std::string>                       list;
    Expressions                                    explist;
    std::pair<std::shared_ptr<SQLExpression>,bool> orderexp;
    std::pair<Expressions, std::vector<bool>>      orderlist;
    std::vector<std::reference_wrapper<SQLTable>>  tablist;
    ColumnDef                                      coldef;
    std::vector<ColumnDef>                         coldefs;
    std::pair<std::string, int>                    bfdef;
    std::vector<std::pair<std::string, int>>       bfdefs;
    bool                                           bol;
    std::pair<long, long>                          range;
};

#ifdef YY_DECL
#undef YY_DECL
#endif
#define YY_DECL int eckit_sql_lex (YYSTYPE *eckit_sql_lval_param, eckit_sql_scan_t eckit_sql_scanner, eckit::sql::SQLSession*)

#define YYSTYPE_IS_DECLARED 1

YY_DECL;

Expressions emptyExpressionList;

%}

%token <val>STRING
%token <val>IDENT
%token <val>VAR

%token <num>DOUBLE

%token SEMICOLON
%token LINK
%token TYPEOF
%token READONLY
%token UPDATED
%token NOREORDER
%token SAFEGUARD

%token EXIT

%token SELECT
%token INTO
%token FROM
%token SET
%token DATABASE
%token COUNT
%token WHERE
%token GROUP
%token ORDER
%token BY
%token INSERT
%token VALUES

%token CREATE
%token SCHEMA
%token VIEW
%token INDEX
%token TABLE
%token TYPE
%token TYPEDEF
%token TEMPORARY
%token INHERITS
%token DEFAULT

%token CONSTRAINT
%token UNIQUE
%token PRIMARY
%token FOREIGN
%token KEY
%token REFERENCES

%token EQ
%token GE
%token LE
%token NE
%token IN
%token NOT
%token AND
%token OR
%token ON
%token IS
%token AS
%token NIL
%token ALL
%token DISTINCT
%token BETWEEN

%token ASC
%token DESC

%token HASH
%token LIKE
%token RLIKE
%token MATCH
%token QUERY
%token ALIGN
%token RESET
%token DUAL
%token ONELOOPER

%type <exp>     expression assignment_rhs;
%type <exp>     column factor term conjonction disjonction condition atom_or_number vector_index optional_hash;
%type <exp>     where;
%type <explist> expression_list group_by;

%type <tablist> table_list;
%type <table> table
%type <val> table_reference;
%type <tablist> from;

%type <orderlist> order_by order_list;
%type <orderexp> order;

%type <explist> select_list select_list_;
%type <exp> select select_;

%type <val> into;
%type <val> func relational_operator;

%type <val> data_type column_name table_name;
%type <coldef> column_def;
%type <coldefs> column_def_list column_def_list_;
%type <bfdef> bitfield_def
%type <bfdefs> bitfield_def_list bitfield_def_list_

%type <bol> distinct;

%type <val> bitfield_ref default_value;

%type <range> vector_range_decl;

%%

start : statements { session->currentDatabase().setLinks(); }
    ;

statements : statement ';'
           | statements statement ';'
           ;

statement: select_statement
//		 | create_view_statement   { session->statement($1); }
//		 | insert_statement        { session->statement(session->insertFactory().create(*session, /*InsertAST* */ ($1))); }
         | set_statement
//		 | create_schema_statement
//		 | create_index_statement
         | create_type_statement
         | create_table_statement
//		 | readonly_statement
//		 | updated_statement
//		 | noreorder_statement
//		 | safeguard_statement
//		 | exit_statement
//		 | align_statement
//		 | reset_statement
//		 | onelooper_statement
//		 | error
         | empty
    ;
//
//exit_statement: EXIT { return 0; }
//	;
//
//readonly_statement: READONLY
//	;
//
//updated_statement: UPDATED
//	;
//
//noreorder_statement: NOREORDER
//	;
//
//safeguard_statement: SAFEGUARD
//	;
//
//align_statement: ALIGN '(' IDENT ',' IDENT ')' ;
//onelooper_statement: ONELOOPER '(' IDENT ',' IDENT ')' ;
//reset_statement: RESET ALIGN ;
//reset_statement: RESET ONELOOPER ;
//
//create_schema_statement: CREATE SCHEMA schema_name schema_element_list
//        {
//            session->currentDatabase().schemaAnalyzer().endSchema();
//        }
//        ;
//
//schema_name: IDENT
//        {
//           session->currentDatabase().schemaAnalyzer().beginSchema($1);
//        }
//       ;
//
//schema_element_list: schema_element
//                   | schema_element_list schema_element
//                   ;
//
//schema_element: create_table_statement
//              | create_view_statement
//              | empty
//              ;
//
//create_index_statement: CREATE INDEX IDENT TABLE IDENT
//	{
//        session->createIndex($3,$5);
//	}
//	| CREATE INDEX IDENT '@' IDENT
//	{
//        session->createIndex($3,$5);
//	}
//	;

create_type_statement: create_type IDENT as_or_eq '(' bitfield_def_list ')'
    {
        std::string typeName ($2);
        std::vector<std::pair<std::string, int>> bfDefs($5);
        FieldNames	fields;
        Sizes		sizes;

        for (const auto& def : bfDefs) {

            const std::string& name(def.first);
            const int& size(def.second);
            fields.push_back(name);
            sizes.push_back(size);
            ASSERT(sizes.back() > 0);
        }
        // std::string typeSignature (type::SQLBitfield::make("Bitfield", fields, sizes, typeName.c_str()));

        session->currentDatabase()
        .schemaAnalyzer().addBitfieldType(typeName, fields, sizes); //, typeSignature);

        //cout << "CREATE TYPE " << typeName << " AS " << typeSignature << ";" << std::endl;
    }
    ;

create_type_statement: create_type IDENT as_or_eq IDENT
    {
        type::SQLType::createAlias($4, $2);
    }
    ;

create_type: CREATE TYPE
           | TYPEDEF
           ;

as_or_eq: AS
        | EQ
        ;

bitfield_def_list: bitfield_def_list_ { $$ = $1; }
                 | bitfield_def_list_ ',' { $$ = $1; }
    ;

bitfield_def_list_: bitfield_def { $$ = {}; $$.push_back($1); }
                  | bitfield_def_list_ ',' bitfield_def { $$ = $1; $$.push_back($3); }
    ;

bitfield_def: column_name data_type
    {
        std::string name($1);
        std::string sz($2);

        if ((sz.size() != 4 && sz.size() != 5) || sz.substr(0, 3) != "bit") {
            std::ostringstream ss;
            ss << "Unexpected bitfield definition: " << name << " --> " << sz;
            throw eckit::UserError(ss.str(), Here());
        }

        int size = ::strtod(sz.c_str()+3, nullptr);
        ASSERT(size > 0);

        $$ = std::make_pair(name, size);
    }
    ;

//temporary: TEMPORARY  { $$ = true; }
//         | empty      { $$ = false; }
//         ;
//
//inheritance_list: inheritance_list_       { $$ = $1; }
//                 | inheritance_list_ ','  { $$ = $1; }
//                 ;
//
//inheritance_list_: IDENT                         { $$ = std::vector<std::string>(); $$.insert($$.begin(), $1); }
//                 | inheritance_list_ ',' IDENT   { $$ = $1; $$.push_back($3); }
//
//inherits: INHERITS '(' inheritance_list ')'   { $$ = $3;               }
//        | empty                               { $$ = std::vector<std::string>(); }
//        ;
//
//constraint_list: constraint_list_ { $$ = $1; }
//               | constraint_list_ ',' { $$ = $1; }
//               ;
//
//constraint_list_: constraint { $$ = ConstraintDefs(1, $1); }
//                | constraint_list_ ',' constraint { $$ = $1; $$.push_back($3); }
//                | empty { $$ = ConstraintDefs(0); }
//                ;
//
//constraint: primary_key { $$ = $1; }
//          | foreign_key { $$ = $1; }
//          ;
//
//primary_key: constraint_name UNIQUE '(' column_reference_list ')' { $$ = ConstraintDef($1, $4); }
//           | constraint_name PRIMARY KEY '(' column_reference_list ')' { $$ = ConstraintDef($1, $5); }
//           ;
//
//foreign_key: constraint_name FOREIGN KEY '(' column_reference_list ')' REFERENCES IDENT '(' column_reference_list ')'
//           {
//                $$ = ConstraintDef($1, $5, $8, $10);
//           }
//           ;
//
//constraint_name: CONSTRAINT IDENT { $$ = $2; }
//               | empty { $$ = std::string(); }
//               ;
//
//column_reference_list: column_reference { $$ = std::vector<std::string>(1, $1); }
//                | column_reference_list ',' column_reference { $$ = $1; $$.push_back($3); }
//                ;
//
//column_reference: IDENT table_reference { $$ = $1 + $2; }
//           ;
//
//location: ON STRING { $$ = $2; }
//        | empty     { $$ = ""; }
//        ;

// table_md: AS '(' column_def_list ')' { $$ = $3; }             //constraint_list ')' { $$ = make_pair($3, $4); }
//         | empty                      { $$ = ColumnDefs(); }   //make_pair(ColumnDefs(), ConstraintDefs()); }
//         ;

//create_table_statement: CREATE temporary TABLE table_name table_md inherits location

create_table_statement: CREATE TABLE table_name AS '(' column_def_list ')'
    {
        std::string name = $3;
        ColumnDefs cols = $6;

        TableDef tableDef(name, cols);
        session->currentDatabase().schemaAnalyzer().addTable(tableDef);
    }
    ;

// TODO: Do we need ot be able to construct a table name from an expression?

table_name: IDENT { $$ = $1; }
          | IDENT '.' IDENT { $$ = $1 + std::string(".") + $3; }
          | expression { std::shared_ptr<SQLExpression> e($1); $$ = e->title(); }
          ;

column_def_list: column_def_list_     { $$ = $1; }
               | column_def_list_ ',' { $$ = $1; }
               | empty                { $$ = ColumnDefs(); }
               ;

column_def_list_: column_def                      { $$ = ColumnDefs(); $$.push_back($1); }
                | column_def_list_ ',' column_def { $$ = $1; $$.push_back($3); }
                ;

column_def: column_name vector_range_decl data_type default_value
    {
        // n.b. Ignore vector range decl and default value. These are just for valid schema parsing
        std::string column_name($1);
        std::string data_type($3);
        const BitfieldDef& bitfield(session->currentDatabase().schemaAnalyzer().getBitfieldType(data_type));
        $$ = ColumnDef(column_name, data_type, bitfield);
    }
    ;

vector_range_decl: '[' DOUBLE ']'            { $$ = std::make_pair(1, $2); }
                 | '[' DOUBLE ':' DOUBLE ']' { $$ = std::make_pair($2, $4); }
                 | empty                     { $$ = std::make_pair(0, 0); }
                 ;

column_name: IDENT { $$ = $1; }
           ;

data_type: IDENT                 { $$ = $1; }
         | LINK                  { $$ = "@LINK"; }
//         | TYPEOF '(' column ')' {
//                                    std::stringstream ss;
//                                    ss << *($3);
//                                    std::string columnName (ss.str());
//                                    std::string type (session->currentDatabase().schemaAnalyzer().findColumnType(columnName));
//                                    Log::debug() << "TYPEOF(" << *($3) << ") => " << type << std::endl;
//                                    $$ = type;
//                                 }
         ;

default_value: DEFAULT expression { std::shared_ptr<SQLExpression> e($2); $$ = e->title(); }
             | empty { $$ = std::string(); }
             ;

//create_view_statement: CREATE VIEW IDENT AS select_statement { $$ = $5; }
//	;

select_statement: SELECT distinct select_list into from where group_by order_by
                {
                    bool                                          distinct($2);
                    Expressions                                   select_list($3);
                    std::string                                   into($4);
                    std::vector<std::reference_wrapper<SQLTable>> from ($5);
                    std::shared_ptr<SQLExpression>                where($6);
                    Expressions                                   group_by($7);
                    std::pair<Expressions,std::vector<bool>>      order_by($8);

                    session->setStatement(
                        session->selectFactory().create(distinct, select_list, into, from, where, group_by, order_by)
                    );
                }
                ;

distinct: DISTINCT { $$ = true; }
        | empty    { $$ = false; }
        ;

into: INTO IDENT   { $$ = $2; }
    | INTO STRING  { $$ = $2; }
    | empty        { $$ = ""; }
    ;

from : FROM table_list   { $$ = $2; }
     | empty             { $$ = std::vector<std::reference_wrapper<SQLTable>>(); }
     ;

where : WHERE expression { $$ = $2; }
      |                  { $$ = 0;  }
      ;

//insert_statement: INSERT INTO table optional_columns VALUES values
//                { $$ = InsertAST($3, $4, $6); }
//                ;
//
//optional_columns: '(' columns ')'   { $$ = $2; }
//                | empty             { $$ = std::vector<std::string>(); }
//                ;
//
//columns: IDENT              { $$ = std::vector<std::string>(); $$.insert($$.begin(), $1); }
//       | columns ',' IDENT  { $$ = $1; $$.push_back($3); }
//       ;
//
//values: '(' values_list ')' { $$ = $2; }
//      ;
//
//values_list: '?'                  { $$ = std::vector<std::string>(); $$.insert($$.begin(), "?" /*$1*/); }
//           | values_list ',' '?'  { $$ = $1; $$.push_back("?"/*$3*/); }
//           ;

assignment_rhs	: expression
        ;

//set_statement : SET DATABASE STRING { session->openDatabase($3); }
//	;
//
//set_statement : SET DATABASE STRING AS IDENT { session->openDatabase($3,$5); }
//	;

set_statement : SET VAR EQ assignment_rhs
    {
        //using namespace std;
        //cout << "== set variable " << $2 << " to "; if ($4) cout << *($4) << std::endl; else cout << "NULL" << std::endl;
        session->currentDatabase().setVariable($2, $4);
    }
    ;

bitfield_ref: '.' IDENT  { $$ = $2; }
            |            { $$ = std::string(); }
            ;

column: IDENT vector_index table_reference optional_hash {
            std::string columnName = $1;
            std::shared_ptr<SQLExpression> vectorIndex = $2;
            std::string tableReference($3); // TODO: table_reference should handle .<database> suffix
            std::shared_ptr<SQLExpression> pshift($4);

            std::string bitfieldName;
            $$ = session->selectFactory().createColumn(columnName, bitfieldName, vectorIndex, tableReference /*TODO: handle .<database> */, pshift);
         }
         | IDENT bitfield_ref table_reference optional_hash {

            std::string columnName = $1;
            std::string bitfieldName = $2;
            std::shared_ptr<SQLExpression> vectorIndex;
            std::string tableReference($3); // TODO: table_reference should handle .<database> suffix
            std::shared_ptr<SQLExpression> pshift       ($4);

            $$ = session->selectFactory().createColumn(columnName, bitfieldName, vectorIndex, tableReference /*TODO: handle .<database> */, pshift);
         }
      ;

vector_index : '[' expression ']'    { $$ = $2; }
             | empty                 { $$ = NULL; }
             ;

table_reference: '@' IDENT   { $$ = std::string("@") + $2; }
               | empty       { $$ = std::string(""); }
               ;

// n.b. convert SQLTable& into SQLTable* as we need to store a ptr to keep YACC happy
// table : IDENT '.' IDENT { $$ = &session->findTable($1, $3);  // Multiple databases not supported
table : DUAL            { $$ = 0; }
      | IDENT           { $$ = &session->findTable($1); }
      | STRING			{ $$ = &session->findTable($1); }
      ;

table_list : table                  { $$ = std::vector<std::reference_wrapper<SQLTable>>(); if($1) { $$.push_back(*$1); } }
           | table_list  ',' table  { $$ = $1; if ($3) { $$.push_back(*$3); } }
           ;

///*================= SELECT =========================================*/

select_list: select_list_     { $$ = $1; }
           | select_list_ ',' { $$ = $1; }
           ;

select_list_ : select         { $$ = Expressions(); $$.push_back($1); }
    | select_list_ ',' select { $$ = $1; $$.push_back($3); }
    ;

select: select_ access_decl { $$ = $1; }
      ;

select_: '*' table_reference                             { $$ = std::make_shared<ColumnExpression>("*", $2);  }
       | IDENT '.' '*' table_reference                   { $$ = std::make_shared<BitColumnExpression>($1, "*", $4); }
//	   | IDENT '[' expression ':' expression ']' table
//		{
//			// TODO: Add simillar rule for BitColumnExpression.
//			bool missing (false);
//			int begin ($3->eval(missing)); //ASSERT(!missing);
//			int end ($5->eval(missing)); //ASSERT(!missing);
//            Table table ($7);
//			$$ = std::make_shared<ColumnExpression>($1, table.name /*TODO: handle .<database> */, begin, end);
//		}
       | expression AS IDENT table_reference { $$ = $1; $$->title($3 + $4); }
       | expression
       ;

access_decl: UPDATED
           | READONLY
           | empty
           ;

///*================= GROUP BY ======================================*/

group_by: GROUP BY expression_list { $$ = $3;                       }
      | empty                    { $$ = Expressions(); }
        ;

///*================= ORDER =========================================*/

order_by: ORDER BY order_list { $$ = $3;                       }
        | empty               { $$ = std::make_pair(Expressions(),std::vector<bool>()); }
        ;


order_list : order                  { $$ = std::make_pair(Expressions(), std::vector<bool>()); $$.first.push_back($1.first); $$.second.push_back($1.second); }
           | order_list ',' order   { $$ = $1; $$.first.push_back($3.first); $$.second.push_back($3.second); }
           ;

order : expression DESC      { $$ = std::make_pair($1, false); }
      | expression ASC       { $$ = std::make_pair($1, true); }
      | expression			 { $$ = std::make_pair($1, true); }
      ;


/*================= EXPRESSION =========================================*/

expression_list : expression         {  $$ = Expressions(); $$.push_back($1); }
                | expression_list ',' expression { $$ = $1; $$.push_back($3); }
                ;

optional_hash : HASH DOUBLE { $$ = std::make_shared<NumberExpression>($2); }
              |             { $$ = std::make_shared<NumberExpression>(0); }
              ;


atom_or_number : '(' expression ')'           { $$ = $2; }
               | '-' expression               { $$ = FunctionFactory::instance().build("-",$2); }
               | DOUBLE                       { $$ = std::make_shared<NumberExpression>($1); }
               |
               column
               | VAR                          { $$ = session->currentDatabase().getVariable($1); }
               | '?' DOUBLE                   { $$ = std::make_shared<ParameterExpression>($2); }
               | func '(' expression_list ')' { $$ = FunctionFactory::instance().build($1, $3); }
               | func '(' empty ')'           { $$ = FunctionFactory::instance().build($1, emptyExpressionList); }
               | func '(' '*' ')'
                {
                    if (std::string("count") != $1)
                        throw eckit::UserError(std::string("Only function COUNT can accept '*' as parameter (") + $1 + ")");

                    $$ = FunctionFactory::instance().build("count", std::make_shared<NumberExpression>(1.0));
                }
               | STRING                       { $$ = std::make_shared<StringExpression>($1); }
               ;


func : IDENT { $$ = $1;      }
     | COUNT { $$ = "count"; }
     ;

/* note: a^b^c -> a^(b^c) as in fortran */

factor      : factor '*' atom_or_number          { $$ = FunctionFactory::instance().build("*",$1,$3);   }
            | factor '/' atom_or_number          { $$ = FunctionFactory::instance().build("/",$1,$3); }
            /* | factor '%' atom_or_number          { $$ = std::make_shared<CondMOD>($1,$3); } */
            | atom_or_number
            ;

term        : term '+' factor           { $$ = FunctionFactory::instance().build("+",$1,$3);   }
            | term '-' factor           { $$ = FunctionFactory::instance().build("-",$1,$3);   }
            /* | term '&' factor */
            | factor
            ;

relational_operator: '>' { $$ = ">"; }
                   | EQ  { $$ = "="; }
                   | '<' { $$ = "<"; }
                   | GE  { $$ = ">="; }
                   | LE  { $$ = "<="; }
                   | NE  { $$ = "<>"; }
                   ;

condition   : term relational_operator term relational_operator term { $$ = FunctionFactory::instance().build("and", FunctionFactory::instance().build($2,$1,$3), FunctionFactory::instance().build($4,$3,$5)); }
            | term relational_operator term                          { $$ = FunctionFactory::instance().build($2, $1, $3); }
            | MATCH '(' expression_list ')' IN QUERY '(' select_statement ')'
            {
                 NOTIMP;
            //    const Expressions& matchList ($3);
            //    const SelectAST& subquery ($8);
            //    $$ = FunctionFactory::instance().build("match", matchList, subquery);
            }
            | condition  IN '(' expression_list ')'                  { $4.push_back($1); $$ = FunctionFactory::instance().build("in",$4);   }
            | condition  NOT IN '(' expression_list ')'  { $5.push_back($1); $$ = FunctionFactory::instance().build("not_in",$5);   }
            | NOT condition             { $$ = FunctionFactory::instance().build("not",$2);   }
            | condition IS NIL          { $$ = FunctionFactory::instance().build("null",$1);   }
            | condition IS NOT NIL      { $$ = FunctionFactory::instance().build("not_null",$1);   }
            | condition BETWEEN term AND term { $$ = FunctionFactory::instance().build("between",$1,$3,$5); }
            | condition NOT BETWEEN term AND term { $$ = FunctionFactory::instance().build("not_between",$1,$4,$6); }
            | condition LIKE term       { $$ = FunctionFactory::instance().build("like", $1, $3); }
            | condition RLIKE term      { $$ = FunctionFactory::instance().build("rlike", $1, $3); }
            | term
            ;

conjonction : conjonction AND condition       { $$ = FunctionFactory::instance().build("and",$1,$3);   }
            | condition
            ;

disjonction : disjonction OR conjonction      { $$ = FunctionFactory::instance().build("or",$1,$3);   }
            | conjonction
            ;


expression  : disjonction
            | expression '[' expression ']'
            {
                // This has not been implemented yet.
                // SDS: n.b. seems to be in old versions. Not sure why deprecated
                throw UserError("Syntax: 'expression [ expression ]' not yet supported");
            }
            ;

empty :
      ;

%%
#undef YY_NULL
#include "sqll.c"

