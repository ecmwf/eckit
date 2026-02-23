/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or eckit_sql__.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_RUNNER_WORK_ECKIT_ECKIT_CODEQL_BUILD_DIR_SRC_ECKIT_SQL_SQLY_TMP_H_INCLUDED
# define YY_YY_HOME_RUNNER_WORK_ECKIT_ECKIT_CODEQL_BUILD_DIR_SRC_ECKIT_SQL_SQLY_TMP_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int eckit_sql_debug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum eckit_sql_tokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    STRING = 258,                  /* STRING  */
    IDENT = 259,                   /* IDENT  */
    VAR = 260,                     /* VAR  */
    DOUBLE = 261,                  /* DOUBLE  */
    SEMICOLON = 262,               /* SEMICOLON  */
    LINK = 263,                    /* LINK  */
    TYPEOF = 264,                  /* TYPEOF  */
    READONLY = 265,                /* READONLY  */
    UPDATED = 266,                 /* UPDATED  */
    NOREORDER = 267,               /* NOREORDER  */
    SAFEGUARD = 268,               /* SAFEGUARD  */
    EXIT = 269,                    /* EXIT  */
    SELECT = 270,                  /* SELECT  */
    INTO = 271,                    /* INTO  */
    FROM = 272,                    /* FROM  */
    SET = 273,                     /* SET  */
    DATABASE = 274,                /* DATABASE  */
    COUNT = 275,                   /* COUNT  */
    WHERE = 276,                   /* WHERE  */
    GROUP = 277,                   /* GROUP  */
    ORDER = 278,                   /* ORDER  */
    BY = 279,                      /* BY  */
    INSERT = 280,                  /* INSERT  */
    VALUES = 281,                  /* VALUES  */
    CREATE = 282,                  /* CREATE  */
    SCHEMA = 283,                  /* SCHEMA  */
    VIEW = 284,                    /* VIEW  */
    INDEX = 285,                   /* INDEX  */
    TABLE = 286,                   /* TABLE  */
    TYPE = 287,                    /* TYPE  */
    TYPEDEF = 288,                 /* TYPEDEF  */
    TEMPORARY = 289,               /* TEMPORARY  */
    INHERITS = 290,                /* INHERITS  */
    DEFAULT = 291,                 /* DEFAULT  */
    CONSTRAINT = 292,              /* CONSTRAINT  */
    UNIQUE = 293,                  /* UNIQUE  */
    PRIMARY = 294,                 /* PRIMARY  */
    FOREIGN = 295,                 /* FOREIGN  */
    KEY = 296,                     /* KEY  */
    REFERENCES = 297,              /* REFERENCES  */
    EQ = 298,                      /* EQ  */
    GE = 299,                      /* GE  */
    LE = 300,                      /* LE  */
    NE = 301,                      /* NE  */
    IN = 302,                      /* IN  */
    NOT = 303,                     /* NOT  */
    AND = 304,                     /* AND  */
    OR = 305,                      /* OR  */
    ON = 306,                      /* ON  */
    IS = 307,                      /* IS  */
    AS = 308,                      /* AS  */
    NIL = 309,                     /* NIL  */
    ALL = 310,                     /* ALL  */
    DISTINCT = 311,                /* DISTINCT  */
    BETWEEN = 312,                 /* BETWEEN  */
    ASC = 313,                     /* ASC  */
    DESC = 314,                    /* DESC  */
    HASH = 315,                    /* HASH  */
    LIKE = 316,                    /* LIKE  */
    RLIKE = 317,                   /* RLIKE  */
    MATCH = 318,                   /* MATCH  */
    QUERY = 319,                   /* QUERY  */
    ALIGN = 320,                   /* ALIGN  */
    RESET = 321,                   /* RESET  */
    DUAL = 322,                    /* DUAL  */
    ONELOOPER = 323                /* ONELOOPER  */
  };
  typedef enum eckit_sql_tokentype eckit_sql_token_kind_t;
#endif

/* Value type.  */




int eckit_sql_parse (void * scanner, eckit::sql::SQLSession* session);


#endif /* !YY_YY_HOME_RUNNER_WORK_ECKIT_ECKIT_CODEQL_BUILD_DIR_SRC_ECKIT_SQL_SQLY_TMP_H_INCLUDED  */
