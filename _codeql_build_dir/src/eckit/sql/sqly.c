/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or eckit_sql__.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with eckit_sql_ or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 7 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"


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


#line 125 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "sqly.tmp.h"
/* Symbol kind.  */
enum eckit_sql_symbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_STRING = 3,                     /* STRING  */
  YYSYMBOL_IDENT = 4,                      /* IDENT  */
  YYSYMBOL_VAR = 5,                        /* VAR  */
  YYSYMBOL_DOUBLE = 6,                     /* DOUBLE  */
  YYSYMBOL_SEMICOLON = 7,                  /* SEMICOLON  */
  YYSYMBOL_LINK = 8,                       /* LINK  */
  YYSYMBOL_TYPEOF = 9,                     /* TYPEOF  */
  YYSYMBOL_READONLY = 10,                  /* READONLY  */
  YYSYMBOL_UPDATED = 11,                   /* UPDATED  */
  YYSYMBOL_NOREORDER = 12,                 /* NOREORDER  */
  YYSYMBOL_SAFEGUARD = 13,                 /* SAFEGUARD  */
  YYSYMBOL_EXIT = 14,                      /* EXIT  */
  YYSYMBOL_SELECT = 15,                    /* SELECT  */
  YYSYMBOL_INTO = 16,                      /* INTO  */
  YYSYMBOL_FROM = 17,                      /* FROM  */
  YYSYMBOL_SET = 18,                       /* SET  */
  YYSYMBOL_DATABASE = 19,                  /* DATABASE  */
  YYSYMBOL_COUNT = 20,                     /* COUNT  */
  YYSYMBOL_WHERE = 21,                     /* WHERE  */
  YYSYMBOL_GROUP = 22,                     /* GROUP  */
  YYSYMBOL_ORDER = 23,                     /* ORDER  */
  YYSYMBOL_BY = 24,                        /* BY  */
  YYSYMBOL_INSERT = 25,                    /* INSERT  */
  YYSYMBOL_VALUES = 26,                    /* VALUES  */
  YYSYMBOL_CREATE = 27,                    /* CREATE  */
  YYSYMBOL_SCHEMA = 28,                    /* SCHEMA  */
  YYSYMBOL_VIEW = 29,                      /* VIEW  */
  YYSYMBOL_INDEX = 30,                     /* INDEX  */
  YYSYMBOL_TABLE = 31,                     /* TABLE  */
  YYSYMBOL_TYPE = 32,                      /* TYPE  */
  YYSYMBOL_TYPEDEF = 33,                   /* TYPEDEF  */
  YYSYMBOL_TEMPORARY = 34,                 /* TEMPORARY  */
  YYSYMBOL_INHERITS = 35,                  /* INHERITS  */
  YYSYMBOL_DEFAULT = 36,                   /* DEFAULT  */
  YYSYMBOL_CONSTRAINT = 37,                /* CONSTRAINT  */
  YYSYMBOL_UNIQUE = 38,                    /* UNIQUE  */
  YYSYMBOL_PRIMARY = 39,                   /* PRIMARY  */
  YYSYMBOL_FOREIGN = 40,                   /* FOREIGN  */
  YYSYMBOL_KEY = 41,                       /* KEY  */
  YYSYMBOL_REFERENCES = 42,                /* REFERENCES  */
  YYSYMBOL_EQ = 43,                        /* EQ  */
  YYSYMBOL_GE = 44,                        /* GE  */
  YYSYMBOL_LE = 45,                        /* LE  */
  YYSYMBOL_NE = 46,                        /* NE  */
  YYSYMBOL_IN = 47,                        /* IN  */
  YYSYMBOL_NOT = 48,                       /* NOT  */
  YYSYMBOL_AND = 49,                       /* AND  */
  YYSYMBOL_OR = 50,                        /* OR  */
  YYSYMBOL_ON = 51,                        /* ON  */
  YYSYMBOL_IS = 52,                        /* IS  */
  YYSYMBOL_AS = 53,                        /* AS  */
  YYSYMBOL_NIL = 54,                       /* NIL  */
  YYSYMBOL_ALL = 55,                       /* ALL  */
  YYSYMBOL_DISTINCT = 56,                  /* DISTINCT  */
  YYSYMBOL_BETWEEN = 57,                   /* BETWEEN  */
  YYSYMBOL_ASC = 58,                       /* ASC  */
  YYSYMBOL_DESC = 59,                      /* DESC  */
  YYSYMBOL_HASH = 60,                      /* HASH  */
  YYSYMBOL_LIKE = 61,                      /* LIKE  */
  YYSYMBOL_RLIKE = 62,                     /* RLIKE  */
  YYSYMBOL_MATCH = 63,                     /* MATCH  */
  YYSYMBOL_QUERY = 64,                     /* QUERY  */
  YYSYMBOL_ALIGN = 65,                     /* ALIGN  */
  YYSYMBOL_RESET = 66,                     /* RESET  */
  YYSYMBOL_DUAL = 67,                      /* DUAL  */
  YYSYMBOL_ONELOOPER = 68,                 /* ONELOOPER  */
  YYSYMBOL_69_ = 69,                       /* ';'  */
  YYSYMBOL_70_ = 70,                       /* '('  */
  YYSYMBOL_71_ = 71,                       /* ')'  */
  YYSYMBOL_72_ = 72,                       /* ','  */
  YYSYMBOL_73_ = 73,                       /* '.'  */
  YYSYMBOL_74_ = 74,                       /* '['  */
  YYSYMBOL_75_ = 75,                       /* ']'  */
  YYSYMBOL_76_ = 76,                       /* ':'  */
  YYSYMBOL_77_ = 77,                       /* '@'  */
  YYSYMBOL_78_ = 78,                       /* '*'  */
  YYSYMBOL_79_ = 79,                       /* '-'  */
  YYSYMBOL_80_ = 80,                       /* '?'  */
  YYSYMBOL_81_ = 81,                       /* '/'  */
  YYSYMBOL_82_ = 82,                       /* '+'  */
  YYSYMBOL_83_ = 83,                       /* '>'  */
  YYSYMBOL_84_ = 84,                       /* '<'  */
  YYSYMBOL_YYACCEPT = 85,                  /* $accept  */
  YYSYMBOL_start = 86,                     /* start  */
  YYSYMBOL_statements = 87,                /* statements  */
  YYSYMBOL_statement = 88,                 /* statement  */
  YYSYMBOL_create_type_statement = 89,     /* create_type_statement  */
  YYSYMBOL_create_type = 90,               /* create_type  */
  YYSYMBOL_as_or_eq = 91,                  /* as_or_eq  */
  YYSYMBOL_bitfield_def_list = 92,         /* bitfield_def_list  */
  YYSYMBOL_bitfield_def_list_ = 93,        /* bitfield_def_list_  */
  YYSYMBOL_bitfield_def = 94,              /* bitfield_def  */
  YYSYMBOL_create_table_statement = 95,    /* create_table_statement  */
  YYSYMBOL_table_name = 96,                /* table_name  */
  YYSYMBOL_column_def_list = 97,           /* column_def_list  */
  YYSYMBOL_column_def_list_ = 98,          /* column_def_list_  */
  YYSYMBOL_column_def = 99,                /* column_def  */
  YYSYMBOL_vector_range_decl = 100,        /* vector_range_decl  */
  YYSYMBOL_column_name = 101,              /* column_name  */
  YYSYMBOL_data_type = 102,                /* data_type  */
  YYSYMBOL_default_value = 103,            /* default_value  */
  YYSYMBOL_select_statement = 104,         /* select_statement  */
  YYSYMBOL_distinct = 105,                 /* distinct  */
  YYSYMBOL_into = 106,                     /* into  */
  YYSYMBOL_from = 107,                     /* from  */
  YYSYMBOL_where = 108,                    /* where  */
  YYSYMBOL_assignment_rhs = 109,           /* assignment_rhs  */
  YYSYMBOL_set_statement = 110,            /* set_statement  */
  YYSYMBOL_bitfield_ref = 111,             /* bitfield_ref  */
  YYSYMBOL_column = 112,                   /* column  */
  YYSYMBOL_vector_index = 113,             /* vector_index  */
  YYSYMBOL_table_reference = 114,          /* table_reference  */
  YYSYMBOL_table = 115,                    /* table  */
  YYSYMBOL_table_list = 116,               /* table_list  */
  YYSYMBOL_select_list = 117,              /* select_list  */
  YYSYMBOL_select_list_ = 118,             /* select_list_  */
  YYSYMBOL_select = 119,                   /* select  */
  YYSYMBOL_select_ = 120,                  /* select_  */
  YYSYMBOL_access_decl = 121,              /* access_decl  */
  YYSYMBOL_group_by = 122,                 /* group_by  */
  YYSYMBOL_order_by = 123,                 /* order_by  */
  YYSYMBOL_order_list = 124,               /* order_list  */
  YYSYMBOL_order = 125,                    /* order  */
  YYSYMBOL_expression_list = 126,          /* expression_list  */
  YYSYMBOL_optional_hash = 127,            /* optional_hash  */
  YYSYMBOL_atom_or_number = 128,           /* atom_or_number  */
  YYSYMBOL_func = 129,                     /* func  */
  YYSYMBOL_factor = 130,                   /* factor  */
  YYSYMBOL_term = 131,                     /* term  */
  YYSYMBOL_relational_operator = 132,      /* relational_operator  */
  YYSYMBOL_condition = 133,                /* condition  */
  YYSYMBOL_conjonction = 134,              /* conjonction  */
  YYSYMBOL_disjonction = 135,              /* disjonction  */
  YYSYMBOL_expression = 136,               /* expression  */
  YYSYMBOL_empty = 137                     /* empty  */
};
typedef enum eckit_sql_symbol_kind_t eckit_sql_symbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ eckit_sql_type_int8;
#elif defined YY_STDINT_H
typedef int_least8_t eckit_sql_type_int8;
#else
typedef signed char eckit_sql_type_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ eckit_sql_type_int16;
#elif defined YY_STDINT_H
typedef int_least16_t eckit_sql_type_int16;
#else
typedef short eckit_sql_type_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ eckit_sql_type_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t eckit_sql_type_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char eckit_sql_type_uint8;
#else
typedef short eckit_sql_type_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ eckit_sql_type_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t eckit_sql_type_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short eckit_sql_type_uint16;
#else
typedef int eckit_sql_type_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef eckit_sql_type_uint8 eckit_sql__state_t;

/* State numbers in computations.  */
typedef int eckit_sql__state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about eckit_sql_lval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined eckit_sql_overflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined eckit_sql_overflow */

#if (! defined eckit_sql_overflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union eckit_sql_alloc
{
  eckit_sql__state_t eckit_sql_ss_alloc;
  YYSTYPE eckit_sql_vs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union eckit_sql_alloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (eckit_sql__state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T eckit_sql_newbytes;                                         \
        YYCOPY (&eckit_sql_ptr->Stack_alloc, Stack, eckit_sql_size);                    \
        Stack = &eckit_sql_ptr->Stack_alloc;                                    \
        eckit_sql_newbytes = eckit_sql_stacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        eckit_sql_ptr += eckit_sql_newbytes / YYSIZEOF (*eckit_sql_ptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T eckit_sql_i;                      \
          for (eckit_sql_i = 0; eckit_sql_i < (Count); eckit_sql_i++)   \
            (Dst)[eckit_sql_i] = (Src)[eckit_sql_i];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  20
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   252

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  85
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  53
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  234

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   323


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by eckit_sql_lex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (eckit_sql_symbol_kind_t, eckit_sql_translate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by eckit_sql_lex.  */
static const eckit_sql_type_int8 eckit_sql_translate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      70,    71,    78,    82,    72,    79,    73,    81,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    76,    69,
      84,     2,    83,    80,    77,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    74,     2,    75,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const eckit_sql_type_int16 eckit_sql_rline[] =
{
       0,   170,   170,   173,   174,   177,   180,   183,   184,   194,
     248,   272,   278,   279,   282,   283,   286,   287,   290,   291,
     294,   371,   383,   384,   385,   388,   389,   390,   393,   394,
     397,   407,   408,   409,   412,   415,   416,   427,   428,   434,
     450,   451,   454,   455,   456,   459,   460,   463,   464,   486,
     495,   503,   504,   507,   516,   528,   529,   532,   533,   538,
     539,   540,   543,   544,   549,   550,   553,   554,   557,   560,
     561,   571,   572,   575,   576,   577,   582,   583,   588,   589,
     593,   594,   597,   598,   599,   605,   606,   609,   610,   614,
     615,   616,   618,   619,   620,   621,   622,   623,   630,   634,
     635,   640,   641,   643,   646,   647,   649,   652,   653,   654,
     655,   656,   657,   660,   661,   662,   669,   670,   671,   672,
     673,   674,   675,   676,   677,   678,   681,   682,   685,   686,
     690,   691,   699
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (eckit_sql_symbol_kind_t, eckit_sql_stos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *eckit_sql_symbol_name (eckit_sql_symbol_kind_t eckit_sql_symbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const eckit_sql_tname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "STRING", "IDENT",
  "VAR", "DOUBLE", "SEMICOLON", "LINK", "TYPEOF", "READONLY", "UPDATED",
  "NOREORDER", "SAFEGUARD", "EXIT", "SELECT", "INTO", "FROM", "SET",
  "DATABASE", "COUNT", "WHERE", "GROUP", "ORDER", "BY", "INSERT", "VALUES",
  "CREATE", "SCHEMA", "VIEW", "INDEX", "TABLE", "TYPE", "TYPEDEF",
  "TEMPORARY", "INHERITS", "DEFAULT", "CONSTRAINT", "UNIQUE", "PRIMARY",
  "FOREIGN", "KEY", "REFERENCES", "EQ", "GE", "LE", "NE", "IN", "NOT",
  "AND", "OR", "ON", "IS", "AS", "NIL", "ALL", "DISTINCT", "BETWEEN",
  "ASC", "DESC", "HASH", "LIKE", "RLIKE", "MATCH", "QUERY", "ALIGN",
  "RESET", "DUAL", "ONELOOPER", "';'", "'('", "')'", "','", "'.'", "'['",
  "']'", "':'", "'@'", "'*'", "'-'", "'?'", "'/'", "'+'", "'>'", "'<'",
  "$accept", "start", "statements", "statement", "create_type_statement",
  "create_type", "as_or_eq", "bitfield_def_list", "bitfield_def_list_",
  "bitfield_def", "create_table_statement", "table_name",
  "column_def_list", "column_def_list_", "column_def", "vector_range_decl",
  "column_name", "data_type", "default_value", "select_statement",
  "distinct", "into", "from", "where", "assignment_rhs", "set_statement",
  "bitfield_ref", "column", "vector_index", "table_reference", "table",
  "table_list", "select_list", "select_list_", "select", "select_",
  "access_decl", "group_by", "order_by", "order_list", "order",
  "expression_list", "optional_hash", "atom_or_number", "func", "factor",
  "term", "relational_operator", "condition", "conjonction", "disjonction",
  "expression", "empty", YY_NULLPTR
};

static const char *
eckit_sql_symbol_name (eckit_sql_symbol_kind_t eckit_sql_symbol)
{
  return eckit_sql_tname[eckit_sql_symbol];
}
#endif

#define YYPACT_NINF (-129)

#define eckit_sql_pact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-133)

#define eckit_sql_table_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const eckit_sql_type_int16 eckit_sql_pact[] =
{
     127,   -26,    27,   106,  -129,    39,   126,   -21,  -129,    46,
    -129,  -129,  -129,  -129,  -129,    16,  -129,     9,    95,  -129,
    -129,    -7,  -129,   -18,  -129,    35,  -129,  -129,  -129,   119,
      21,   119,    55,   119,    87,  -129,    81,    34,  -129,    62,
    -129,    42,    29,   140,    19,   108,    76,   -25,   119,   -36,
     125,   113,  -129,  -129,  -129,     7,     5,   119,    55,    55,
    -129,    43,    19,   119,    56,   184,  -129,  -129,   113,  -129,
     145,   173,  -129,    16,  -129,  -129,  -129,  -129,    41,   130,
     130,  -129,  -129,  -129,  -129,   130,   130,  -129,  -129,   130,
     121,   -16,    15,   130,   130,   130,   119,   119,   188,   119,
    -129,   113,   189,   132,  -129,   192,  -129,    55,    77,   137,
     137,   199,    84,   113,  -129,  -129,  -129,  -129,    20,   185,
    -129,  -129,   134,    90,   136,  -129,  -129,    29,    29,   140,
     119,   138,   130,   157,  -129,   -23,    49,    49,    19,   108,
      55,    89,   159,   192,  -129,   142,   143,  -129,    70,  -129,
    -129,   210,  -129,  -129,   171,   119,  -129,  -129,  -129,  -129,
     148,   119,   203,  -129,  -129,  -129,   130,    97,   119,   -22,
    -129,   130,  -129,  -129,   150,   154,  -129,   153,  -129,  -129,
     192,  -129,  -129,  -129,  -129,   164,   113,    20,   113,   205,
     207,  -129,    49,  -129,    99,   130,    49,  -129,   192,   225,
      70,  -129,  -129,   162,  -129,   119,   209,  -129,  -129,  -129,
      49,  -129,   101,   198,   220,   165,   119,  -129,   230,   119,
    -129,  -129,   167,   168,  -129,    44,   166,   113,  -129,   119,
    -129,  -129,  -129,  -129
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const eckit_sql_type_uint8 eckit_sql_defact[] =
{
     132,   132,     0,     0,    13,     0,     2,     0,     7,     0,
       8,     5,     6,     9,    40,     0,    41,     0,     0,    12,
       1,     0,     3,     0,    98,    52,    93,    91,   100,     0,
       0,     0,   132,     0,     0,    92,   132,    64,    66,   132,
     103,     0,   106,   125,   127,   129,   130,    72,     0,    52,
       0,    24,     4,    15,    14,     0,     0,     0,   132,   132,
      56,    52,   118,     0,     0,     0,    69,    58,    90,    94,
       0,   132,    44,    65,    74,    73,    68,    75,   132,     0,
       0,   108,   110,   111,   112,     0,     0,   107,   109,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      50,    49,     0,     0,    11,     0,    51,   132,     0,    88,
      88,     0,     0,    85,    89,    57,    43,    42,     0,    48,
      46,    67,     0,     0,     0,   101,   102,   105,   104,   114,
       0,     0,     0,     0,   119,     0,   123,   124,   126,   128,
     132,     0,    51,   132,    34,     0,    16,    18,     0,    70,
      55,     0,    54,    53,     0,     0,    61,    60,    59,    62,
      45,     0,   132,    97,    95,    96,     0,     0,     0,     0,
     120,     0,    71,   131,     0,    25,    28,   132,    27,    10,
      17,    35,    36,    20,    87,     0,    86,     0,    47,     0,
     132,    77,   113,   116,     0,     0,   121,    21,    26,     0,
       0,    33,    19,     0,    63,     0,     0,    39,    79,   117,
     122,    29,     0,   132,     0,    76,     0,    31,     0,     0,
      30,    38,     0,    78,    80,    84,     0,    37,   115,     0,
      83,    82,    32,    81
};

/* YYPGOTO[NTERM-NUM].  */
static const eckit_sql_type_int16 eckit_sql_pgoto[] =
{
    -129,  -129,  -129,   233,  -129,  -129,  -129,  -129,  -129,    63,
    -129,  -129,  -129,  -129,    47,  -129,  -128,    48,  -129,    28,
    -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,  -129,   -52,
      57,  -129,  -129,  -129,   174,  -129,  -129,  -129,  -129,  -129,
      17,   -76,   139,    93,  -129,    94,   -81,   122,   -28,   155,
    -129,   -15,     4
};

/* YYDEFGOTO[NTERM-NUM].  */
static const eckit_sql_type_uint8 eckit_sql_defgoto[] =
{
       0,     5,     6,     7,     8,     9,    55,   145,   146,   147,
      10,    50,   174,   175,   176,   200,   148,   183,   220,    11,
      15,    71,   119,   162,   100,    12,    58,    35,    59,    66,
     159,   160,    36,    37,    38,    39,    76,   190,   207,   223,
     224,   112,   152,    40,    41,    42,    43,    89,    44,    45,
      46,   113,    67
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const eckit_sql_type_int16 eckit_sql_table[] =
{
      47,    62,   123,    51,    13,    16,   109,   110,   129,   106,
      13,   104,   135,   136,   137,   177,    64,   -22,    68,    24,
      25,    26,    27,   156,   157,    53,   171,   195,    98,    60,
      14,   131,    17,   101,   -99,    54,    28,   102,    57,    20,
      72,   132,   108,    77,    24,    61,    26,    27,    22,    99,
      23,   169,    48,    60,   167,   149,    85,    85,    47,    86,
      86,    28,    52,   133,    29,    60,    90,    91,   138,   134,
     177,    92,    74,    75,   181,   120,    93,   105,   182,    30,
      94,    95,   124,   107,   141,   192,    31,   158,   172,    29,
     196,    63,   194,    69,    32,    33,    34,    70,    24,    49,
      26,    27,   230,   231,    30,   -99,    73,    79,    56,    57,
      80,    31,    78,   -99,   210,    28,   111,    57,    99,   122,
      33,    34,    24,    61,    26,    27,    97,   114,    85,   215,
      99,    86,    65,    24,    61,    26,    27,    18,    19,    28,
     186,     1,     1,    29,     2,     2,   188,   178,   116,   117,
      28,    99,   150,     3,     3,   154,   155,    96,    30,     4,
       4,   164,   155,    99,   173,    31,   191,    29,   193,   155,
     209,   155,   125,   126,    33,    34,   217,   218,   103,   127,
     128,   201,    30,    81,    82,    83,    84,    99,   115,    31,
     118,   130,   140,   142,   208,  -132,   144,   151,    33,    34,
      31,   225,   143,   106,   227,   163,   161,   165,   168,    33,
      34,   170,   -23,   179,   225,   180,   184,   221,   185,    85,
     187,   197,    86,    87,    88,   189,   198,   199,   203,   205,
     206,   212,   214,   216,   219,     1,   226,   155,   228,    21,
     229,   232,   222,   202,   204,   211,   233,   121,   213,   153,
       0,   166,   139
};

static const eckit_sql_type_int16 eckit_sql_check[] =
{
      15,    29,    78,    18,     0,     1,    58,    59,    89,     4,
       6,     4,    93,    94,    95,   143,    31,    53,    33,     3,
       4,     5,     6,     3,     4,    43,    49,    49,    53,    25,
      56,    47,     5,    48,    70,    53,    20,    73,    74,     0,
      36,    57,    57,    39,     3,     4,     5,     6,    69,    74,
       4,   132,    43,    49,   130,   107,    79,    79,    73,    82,
      82,    20,    69,    48,    48,    61,    47,    48,    96,    54,
     198,    52,    10,    11,     4,    71,    57,    70,     8,    63,
      61,    62,    78,    78,    99,   166,    70,    67,   140,    48,
     171,    70,   168,     6,    78,    79,    80,    16,     3,     4,
       5,     6,    58,    59,    63,    70,    72,    78,    73,    74,
      81,    70,    70,    70,   195,    20,    73,    74,    74,    78,
      79,    80,     3,     4,     5,     6,    50,    71,    79,   205,
      74,    82,    77,     3,     4,     5,     6,    31,    32,    20,
     155,    15,    15,    48,    18,    18,   161,   143,     3,     4,
      20,    74,    75,    27,    27,    71,    72,    49,    63,    33,
      33,    71,    72,    74,    75,    70,   162,    48,    71,    72,
      71,    72,    79,    80,    79,    80,    75,    76,    53,    85,
      86,   177,    63,    43,    44,    45,    46,    74,     4,    70,
      17,    70,     4,     4,   190,    69,     4,    60,    79,    80,
      70,   216,    70,     4,   219,    71,    21,    71,    70,    79,
      80,    54,    53,    71,   229,    72,     6,   213,    47,    79,
      72,    71,    82,    83,    84,    22,    72,    74,    64,    24,
      23,     6,    70,    24,    36,    15,     6,    72,    71,     6,
      72,    75,   214,   180,   187,   198,   229,    73,   200,   110,
      -1,   129,    97
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const eckit_sql_type_uint8 eckit_sql_stos[] =
{
       0,    15,    18,    27,    33,    86,    87,    88,    89,    90,
      95,   104,   110,   137,    56,   105,   137,     5,    31,    32,
       0,    88,    69,     4,     3,     4,     5,     6,    20,    48,
      63,    70,    78,    79,    80,   112,   117,   118,   119,   120,
     128,   129,   130,   131,   133,   134,   135,   136,    43,     4,
      96,   136,    69,    43,    53,    91,    73,    74,   111,   113,
     137,     4,   133,    70,   136,    77,   114,   137,   136,     6,
      16,   106,   137,    72,    10,    11,   121,   137,    70,    78,
      81,    43,    44,    45,    46,    79,    82,    83,    84,   132,
      47,    48,    52,    57,    61,    62,    49,    50,    53,    74,
     109,   136,    73,    53,     4,    70,     4,    78,   136,   114,
     114,    73,   126,   136,    71,     4,     3,     4,    17,   107,
     137,   119,    78,   126,   137,   128,   128,   130,   130,   131,
      70,    47,    57,    48,    54,   131,   131,   131,   133,   134,
       4,   136,     4,    70,     4,    92,    93,    94,   101,   114,
      75,    60,   127,   127,    71,    72,     3,     4,    67,   115,
     116,    21,   108,    71,    71,    71,   132,   126,    70,   131,
      54,    49,   114,    75,    97,    98,    99,   101,   137,    71,
      72,     4,     8,   102,     6,    47,   136,    72,   136,    22,
     122,   137,   131,    71,   126,    49,   131,    71,    72,    74,
     100,   137,    94,    64,   115,    24,    23,   123,   137,    71,
     131,    99,     6,   102,    70,   126,    24,    75,    76,    36,
     103,   137,   104,   124,   125,   136,     6,   136,    71,    72,
      58,    59,    75,   125
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const eckit_sql_type_uint8 eckit_sql_r1[] =
{
       0,    85,    86,    87,    87,    88,    88,    88,    88,    88,
      89,    89,    90,    90,    91,    91,    92,    92,    93,    93,
      94,    95,    96,    96,    96,    97,    97,    97,    98,    98,
      99,   100,   100,   100,   101,   102,   102,   103,   103,   104,
     105,   105,   106,   106,   106,   107,   107,   108,   108,   109,
     110,   111,   111,   112,   112,   113,   113,   114,   114,   115,
     115,   115,   116,   116,   117,   117,   118,   118,   119,   120,
     120,   120,   120,   121,   121,   121,   122,   122,   123,   123,
     124,   124,   125,   125,   125,   126,   126,   127,   127,   128,
     128,   128,   128,   128,   128,   128,   128,   128,   128,   129,
     129,   130,   130,   130,   131,   131,   131,   132,   132,   132,
     132,   132,   132,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   134,   134,   135,   135,
     136,   136,   137
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const eckit_sql_type_int8 eckit_sql_r2[] =
{
       0,     2,     1,     2,     3,     1,     1,     1,     1,     1,
       6,     4,     2,     1,     1,     1,     1,     2,     1,     3,
       2,     7,     1,     3,     1,     1,     2,     1,     1,     3,
       4,     3,     5,     1,     1,     1,     1,     2,     1,     8,
       1,     1,     2,     2,     1,     2,     1,     2,     0,     1,
       4,     2,     0,     4,     4,     3,     1,     2,     1,     1,
       1,     1,     1,     3,     1,     2,     1,     3,     2,     2,
       4,     4,     1,     1,     1,     1,     3,     1,     3,     1,
       1,     3,     2,     2,     1,     1,     3,     2,     0,     3,
       2,     1,     1,     1,     2,     4,     4,     4,     1,     1,
       1,     3,     3,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     5,     3,     9,     5,     6,     2,     3,
       4,     5,     6,     3,     3,     1,     3,     1,     3,     1,
       1,     4,     0
};


enum { YYENOMEM = -2 };

#define eckit_sql_errok         (eckit_sql_errstatus = 0)
#define eckit_sql_clearin       (eckit_sql_char = YYEMPTY)

#define YYACCEPT        goto eckit_sql_acceptlab
#define YYABORT         goto eckit_sql_abortlab
#define YYERROR         goto eckit_sql_errorlab
#define YYNOMEM         goto eckit_sql_exhaustedlab


#define YYRECOVERING()  (!!eckit_sql_errstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (eckit_sql_char == YYEMPTY)                                        \
      {                                                           \
        eckit_sql_char = (Token);                                         \
        eckit_sql_lval = (Value);                                         \
        YYPOPSTACK (eckit_sql_len);                                       \
        eckit_sql_state = *eckit_sql_ssp;                                         \
        goto eckit_sql_backup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        eckit_sql_error (scanner, session, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (eckit_sql_debug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (eckit_sql_debug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      eckit_sql__symbol_print (stderr,                                            \
                  Kind, Value, scanner, session); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
eckit_sql__symbol_value_print (FILE *eckit_sql_o,
                       eckit_sql_symbol_kind_t eckit_sql_kind, YYSTYPE const * const eckit_sql_valuep, void * scanner, eckit::sql::SQLSession* session)
{
  FILE *eckit_sql_output = eckit_sql_o;
  YY_USE (eckit_sql_output);
  YY_USE (scanner);
  YY_USE (session);
  if (!eckit_sql_valuep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (eckit_sql_kind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
eckit_sql__symbol_print (FILE *eckit_sql_o,
                 eckit_sql_symbol_kind_t eckit_sql_kind, YYSTYPE const * const eckit_sql_valuep, void * scanner, eckit::sql::SQLSession* session)
{
  YYFPRINTF (eckit_sql_o, "%s %s (",
             eckit_sql_kind < YYNTOKENS ? "token" : "nterm", eckit_sql_symbol_name (eckit_sql_kind));

  eckit_sql__symbol_value_print (eckit_sql_o, eckit_sql_kind, eckit_sql_valuep, scanner, session);
  YYFPRINTF (eckit_sql_o, ")");
}

/*------------------------------------------------------------------.
| eckit_sql__stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
eckit_sql__stack_print (eckit_sql__state_t *eckit_sql_bottom, eckit_sql__state_t *eckit_sql_top)
{
  YYFPRINTF (stderr, "Stack now");
  for (; eckit_sql_bottom <= eckit_sql_top; eckit_sql_bottom++)
    {
      int eckit_sql_bot = *eckit_sql_bottom;
      YYFPRINTF (stderr, " %d", eckit_sql_bot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (eckit_sql_debug)                                                  \
    eckit_sql__stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
eckit_sql__reduce_print (eckit_sql__state_t *eckit_sql_ssp, YYSTYPE *eckit_sql_vsp,
                 int eckit_sql_rule, void * scanner, eckit::sql::SQLSession* session)
{
  int eckit_sql_lno = eckit_sql_rline[eckit_sql_rule];
  int eckit_sql_nrhs = eckit_sql_r2[eckit_sql_rule];
  int eckit_sql_i;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             eckit_sql_rule - 1, eckit_sql_lno);
  /* The symbols being reduced.  */
  for (eckit_sql_i = 0; eckit_sql_i < eckit_sql_nrhs; eckit_sql_i++)
    {
      YYFPRINTF (stderr, "   $%d = ", eckit_sql_i + 1);
      eckit_sql__symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+eckit_sql_ssp[eckit_sql_i + 1 - eckit_sql_nrhs]),
                       &eckit_sql_vsp[(eckit_sql_i + 1) - (eckit_sql_nrhs)], scanner, session);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (eckit_sql_debug)                          \
    eckit_sql__reduce_print (eckit_sql_ssp, eckit_sql_vsp, Rule, scanner, session); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int eckit_sql_debug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
eckit_sql_destruct (const char *eckit_sql_msg,
            eckit_sql_symbol_kind_t eckit_sql_kind, YYSTYPE *eckit_sql_valuep, void * scanner, eckit::sql::SQLSession* session)
{
  YY_USE (eckit_sql_valuep);
  YY_USE (scanner);
  YY_USE (session);
  if (!eckit_sql_msg)
    eckit_sql_msg = "Deleting";
  YY_SYMBOL_PRINT (eckit_sql_msg, eckit_sql_kind, eckit_sql_valuep, eckit_sql_locationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (eckit_sql_kind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| eckit_sql_parse.  |
`----------*/

int
eckit_sql_parse (void * scanner, eckit::sql::SQLSession* session)
{
/* Lookahead token kind.  */
int eckit_sql_char;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE eckit_sql_val_default;)
YYSTYPE eckit_sql_lval YY_INITIAL_VALUE (= eckit_sql_val_default);

    /* Number of syntax errors so far.  */
    int eckit_sql_nerrs = 0;

    eckit_sql__state_fast_t eckit_sql_state = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int eckit_sql_errstatus = 0;

    /* Refer to the stacks through separate pointers, to allow eckit_sql_overflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T eckit_sql_stacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    eckit_sql__state_t eckit_sql_ssa[YYINITDEPTH];
    eckit_sql__state_t *eckit_sql_ss = eckit_sql_ssa;
    eckit_sql__state_t *eckit_sql_ssp = eckit_sql_ss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE eckit_sql_vsa[YYINITDEPTH];
    YYSTYPE *eckit_sql_vs = eckit_sql_vsa;
    YYSTYPE *eckit_sql_vsp = eckit_sql_vs;

  int eckit_sql_n;
  /* The return value of eckit_sql_parse.  */
  int eckit_sql_result;
  /* Lookahead symbol kind.  */
  eckit_sql_symbol_kind_t eckit_sql_token = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE eckit_sql_val;



#define YYPOPSTACK(N)   (eckit_sql_vsp -= (N), eckit_sql_ssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int eckit_sql_len = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  eckit_sql_char = YYEMPTY; /* Cause a token to be read.  */

  goto eckit_sql_setstate;


/*------------------------------------------------------------.
| eckit_sql_newstate -- push a new state, which is found in eckit_sql_state.  |
`------------------------------------------------------------*/
eckit_sql_newstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  eckit_sql_ssp++;


/*--------------------------------------------------------------------.
| eckit_sql_setstate -- set current state (the top of the stack) to eckit_sql_state.  |
`--------------------------------------------------------------------*/
eckit_sql_setstate:
  YYDPRINTF ((stderr, "Entering state %d\n", eckit_sql_state));
  YY_ASSERT (0 <= eckit_sql_state && eckit_sql_state < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *eckit_sql_ssp = YY_CAST (eckit_sql__state_t, eckit_sql_state);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (eckit_sql_ss, eckit_sql_ssp);

  if (eckit_sql_ss + eckit_sql_stacksize - 1 <= eckit_sql_ssp)
#if !defined eckit_sql_overflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T eckit_sql_size = eckit_sql_ssp - eckit_sql_ss + 1;

# if defined eckit_sql_overflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        eckit_sql__state_t *eckit_sql_ss1 = eckit_sql_ss;
        YYSTYPE *eckit_sql_vs1 = eckit_sql_vs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if eckit_sql_overflow is a macro.  */
        eckit_sql_overflow (YY_("memory exhausted"),
                    &eckit_sql_ss1, eckit_sql_size * YYSIZEOF (*eckit_sql_ssp),
                    &eckit_sql_vs1, eckit_sql_size * YYSIZEOF (*eckit_sql_vsp),
                    &eckit_sql_stacksize);
        eckit_sql_ss = eckit_sql_ss1;
        eckit_sql_vs = eckit_sql_vs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= eckit_sql_stacksize)
        YYNOMEM;
      eckit_sql_stacksize *= 2;
      if (YYMAXDEPTH < eckit_sql_stacksize)
        eckit_sql_stacksize = YYMAXDEPTH;

      {
        eckit_sql__state_t *eckit_sql_ss1 = eckit_sql_ss;
        union eckit_sql_alloc *eckit_sql_ptr =
          YY_CAST (union eckit_sql_alloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (eckit_sql_stacksize))));
        if (! eckit_sql_ptr)
          YYNOMEM;
        YYSTACK_RELOCATE (eckit_sql_ss_alloc, eckit_sql_ss);
        YYSTACK_RELOCATE (eckit_sql_vs_alloc, eckit_sql_vs);
#  undef YYSTACK_RELOCATE
        if (eckit_sql_ss1 != eckit_sql_ssa)
          YYSTACK_FREE (eckit_sql_ss1);
      }
# endif

      eckit_sql_ssp = eckit_sql_ss + eckit_sql_size - 1;
      eckit_sql_vsp = eckit_sql_vs + eckit_sql_size - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, eckit_sql_stacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (eckit_sql_ss + eckit_sql_stacksize - 1 <= eckit_sql_ssp)
        YYABORT;
    }
#endif /* !defined eckit_sql_overflow && !defined YYSTACK_RELOCATE */


  if (eckit_sql_state == YYFINAL)
    YYACCEPT;

  goto eckit_sql_backup;


/*-----------.
| eckit_sql_backup.  |
`-----------*/
eckit_sql_backup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  eckit_sql_n = eckit_sql_pact[eckit_sql_state];
  if (eckit_sql_pact_value_is_default (eckit_sql_n))
    goto eckit_sql_default;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (eckit_sql_char == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      eckit_sql_char = eckit_sql_lex (&eckit_sql_lval, scanner, session);
    }

  if (eckit_sql_char <= YYEOF)
    {
      eckit_sql_char = YYEOF;
      eckit_sql_token = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (eckit_sql_char == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      eckit_sql_char = YYUNDEF;
      eckit_sql_token = YYSYMBOL_YYerror;
      goto eckit_sql_errlab1;
    }
  else
    {
      eckit_sql_token = YYTRANSLATE (eckit_sql_char);
      YY_SYMBOL_PRINT ("Next token is", eckit_sql_token, &eckit_sql_lval, &eckit_sql_lloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  eckit_sql_n += eckit_sql_token;
  if (eckit_sql_n < 0 || YYLAST < eckit_sql_n || eckit_sql_check[eckit_sql_n] != eckit_sql_token)
    goto eckit_sql_default;
  eckit_sql_n = eckit_sql_table[eckit_sql_n];
  if (eckit_sql_n <= 0)
    {
      if (eckit_sql_table_value_is_error (eckit_sql_n))
        goto eckit_sql_errlab;
      eckit_sql_n = -eckit_sql_n;
      goto eckit_sql_reduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (eckit_sql_errstatus)
    eckit_sql_errstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", eckit_sql_token, &eckit_sql_lval, &eckit_sql_lloc);
  eckit_sql_state = eckit_sql_n;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++eckit_sql_vsp = eckit_sql_lval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  eckit_sql_char = YYEMPTY;
  goto eckit_sql_newstate;


/*-----------------------------------------------------------.
| eckit_sql_default -- do the default action for the current state.  |
`-----------------------------------------------------------*/
eckit_sql_default:
  eckit_sql_n = eckit_sql_defact[eckit_sql_state];
  if (eckit_sql_n == 0)
    goto eckit_sql_errlab;
  goto eckit_sql_reduce;


/*-----------------------------.
| eckit_sql_reduce -- do a reduction.  |
`-----------------------------*/
eckit_sql_reduce:
  /* eckit_sql_n is the number of a rule to reduce with.  */
  eckit_sql_len = eckit_sql_r2[eckit_sql_n];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  eckit_sql_val = eckit_sql_vsp[1-eckit_sql_len];


  YY_REDUCE_PRINT (eckit_sql_n);
  switch (eckit_sql_n)
    {
  case 2: /* start: statements  */
#line 170 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                   { session->currentDatabase().setLinks(); }
#line 1441 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 10: /* create_type_statement: create_type IDENT as_or_eq '(' bitfield_def_list ')'  */
#line 249 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
    {
        std::string typeName ((eckit_sql_vsp[-4].val));
        std::vector<std::pair<std::string, int>> bfDefs((eckit_sql_vsp[-1].bfdefs));
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
#line 1467 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 11: /* create_type_statement: create_type IDENT as_or_eq IDENT  */
#line 273 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
    {
        type::SQLType::createAlias((eckit_sql_vsp[0].val), (eckit_sql_vsp[-2].val));
    }
#line 1475 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 16: /* bitfield_def_list: bitfield_def_list_  */
#line 286 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                      { (eckit_sql_val.bfdefs) = (eckit_sql_vsp[0].bfdefs); }
#line 1481 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 17: /* bitfield_def_list: bitfield_def_list_ ','  */
#line 287 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                          { (eckit_sql_val.bfdefs) = (eckit_sql_vsp[-1].bfdefs); }
#line 1487 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 18: /* bitfield_def_list_: bitfield_def  */
#line 290 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                 { (eckit_sql_val.bfdefs) = {}; (eckit_sql_val.bfdefs).push_back((eckit_sql_vsp[0].bfdef)); }
#line 1493 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 19: /* bitfield_def_list_: bitfield_def_list_ ',' bitfield_def  */
#line 291 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                        { (eckit_sql_val.bfdefs) = (eckit_sql_vsp[-2].bfdefs); (eckit_sql_val.bfdefs).push_back((eckit_sql_vsp[0].bfdef)); }
#line 1499 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 20: /* bitfield_def: column_name data_type  */
#line 295 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
    {
        std::string name((eckit_sql_vsp[-1].val));
        std::string sz((eckit_sql_vsp[0].val));

        if ((sz.size() != 4 && sz.size() != 5) || sz.substr(0, 3) != "bit") {
            std::ostringstream ss;
            ss << "Unexpected bitfield definition: " << name << " --> " << sz;
            throw eckit::UserError(ss.str(), Here());
        }

        int size = ::strtod(sz.c_str()+3, nullptr);
        ASSERT(size > 0);

        (eckit_sql_val.bfdef) = std::make_pair(name, size);
    }
#line 1519 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 21: /* create_table_statement: CREATE TABLE table_name AS '(' column_def_list ')'  */
#line 372 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
    {
        std::string name = (eckit_sql_vsp[-4].val);
        ColumnDefs cols = (eckit_sql_vsp[-1].coldefs);

        TableDef tableDef(name, cols);
        session->currentDatabase().schemaAnalyzer().addTable(tableDef);
    }
#line 1531 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 22: /* table_name: IDENT  */
#line 383 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                  { (eckit_sql_val.val) = (eckit_sql_vsp[0].val); }
#line 1537 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 23: /* table_name: IDENT '.' IDENT  */
#line 384 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                            { (eckit_sql_val.val) = (eckit_sql_vsp[-2].val) + std::string(".") + (eckit_sql_vsp[0].val); }
#line 1543 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 24: /* table_name: expression  */
#line 385 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                       { std::shared_ptr<SQLExpression> e((eckit_sql_vsp[0].exp)); (eckit_sql_val.val) = e->title(); }
#line 1549 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 25: /* column_def_list: column_def_list_  */
#line 388 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                      { (eckit_sql_val.coldefs) = (eckit_sql_vsp[0].coldefs); }
#line 1555 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 26: /* column_def_list: column_def_list_ ','  */
#line 389 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                      { (eckit_sql_val.coldefs) = (eckit_sql_vsp[-1].coldefs); }
#line 1561 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 27: /* column_def_list: empty  */
#line 390 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                      { (eckit_sql_val.coldefs) = ColumnDefs(); }
#line 1567 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 28: /* column_def_list_: column_def  */
#line 393 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                  { (eckit_sql_val.coldefs) = ColumnDefs(); (eckit_sql_val.coldefs).push_back((eckit_sql_vsp[0].coldef)); }
#line 1573 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 29: /* column_def_list_: column_def_list_ ',' column_def  */
#line 394 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                  { (eckit_sql_val.coldefs) = (eckit_sql_vsp[-2].coldefs); (eckit_sql_val.coldefs).push_back((eckit_sql_vsp[0].coldef)); }
#line 1579 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 30: /* column_def: column_name vector_range_decl data_type default_value  */
#line 398 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
    {
        // n.b. Ignore vector range decl and default value. These are just for valid schema parsing
        std::string column_name((eckit_sql_vsp[-3].val));
        std::string data_type((eckit_sql_vsp[-1].val));
        const BitfieldDef& bitfield(session->currentDatabase().schemaAnalyzer().getBitfieldType(data_type));
        (eckit_sql_val.coldef) = ColumnDef(column_name, data_type, bitfield);
    }
#line 1591 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 31: /* vector_range_decl: '[' DOUBLE ']'  */
#line 407 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                             { (eckit_sql_val.range) = std::make_pair(1, (eckit_sql_vsp[-1].num)); }
#line 1597 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 32: /* vector_range_decl: '[' DOUBLE ':' DOUBLE ']'  */
#line 408 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                             { (eckit_sql_val.range) = std::make_pair((eckit_sql_vsp[-3].num), (eckit_sql_vsp[-1].num)); }
#line 1603 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 33: /* vector_range_decl: empty  */
#line 409 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                             { (eckit_sql_val.range) = std::make_pair(0, 0); }
#line 1609 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 34: /* column_name: IDENT  */
#line 412 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                   { (eckit_sql_val.val) = (eckit_sql_vsp[0].val); }
#line 1615 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 35: /* data_type: IDENT  */
#line 415 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                 { (eckit_sql_val.val) = (eckit_sql_vsp[0].val); }
#line 1621 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 36: /* data_type: LINK  */
#line 416 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                 { (eckit_sql_val.val) = "@LINK"; }
#line 1627 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 37: /* default_value: DEFAULT expression  */
#line 427 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                  { std::shared_ptr<SQLExpression> e((eckit_sql_vsp[0].exp)); (eckit_sql_val.val) = e->title(); }
#line 1633 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 38: /* default_value: empty  */
#line 428 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                     { (eckit_sql_val.val) = std::string(); }
#line 1639 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 39: /* select_statement: SELECT distinct select_list into from where group_by order_by  */
#line 435 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                {
                    bool                                          distinct((eckit_sql_vsp[-6].bol));
                    Expressions                                   select_list((eckit_sql_vsp[-5].explist));
                    std::string                                   into((eckit_sql_vsp[-4].val));
                    std::vector<std::reference_wrapper<SQLTable>> from ((eckit_sql_vsp[-3].tablist));
                    std::shared_ptr<SQLExpression>                where((eckit_sql_vsp[-2].exp));
                    Expressions                                   group_by((eckit_sql_vsp[-1].explist));
                    std::pair<Expressions,std::vector<bool>>      order_by((eckit_sql_vsp[0].orderlist));

                    session->setStatement(
                        session->selectFactory().create(distinct, select_list, into, from, where, group_by, order_by)
                    );
                }
#line 1657 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 40: /* distinct: DISTINCT  */
#line 450 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                   { (eckit_sql_val.bol) = true; }
#line 1663 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 41: /* distinct: empty  */
#line 451 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                   { (eckit_sql_val.bol) = false; }
#line 1669 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 42: /* into: INTO IDENT  */
#line 454 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                   { (eckit_sql_val.val) = (eckit_sql_vsp[0].val); }
#line 1675 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 43: /* into: INTO STRING  */
#line 455 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                   { (eckit_sql_val.val) = (eckit_sql_vsp[0].val); }
#line 1681 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 44: /* into: empty  */
#line 456 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                   { (eckit_sql_val.val) = ""; }
#line 1687 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 45: /* from: FROM table_list  */
#line 459 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.tablist) = (eckit_sql_vsp[0].tablist); }
#line 1693 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 46: /* from: empty  */
#line 460 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.tablist) = std::vector<std::reference_wrapper<SQLTable>>(); }
#line 1699 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 47: /* where: WHERE expression  */
#line 463 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.exp) = (eckit_sql_vsp[0].exp); }
#line 1705 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 48: /* where: %empty  */
#line 464 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.exp) = 0;  }
#line 1711 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 50: /* set_statement: SET VAR EQ assignment_rhs  */
#line 496 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
    {
        //using namespace std;
        //cout << "== set variable " << $2 << " to "; if ($4) cout << *($4) << std::endl; else cout << "NULL" << std::endl;
        session->currentDatabase().setVariable((eckit_sql_vsp[-2].val), (eckit_sql_vsp[0].exp));
    }
#line 1721 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 51: /* bitfield_ref: '.' IDENT  */
#line 503 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.val) = (eckit_sql_vsp[0].val); }
#line 1727 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 52: /* bitfield_ref: %empty  */
#line 504 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.val) = std::string(); }
#line 1733 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 53: /* column: IDENT vector_index table_reference optional_hash  */
#line 507 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                         {
            std::string columnName = (eckit_sql_vsp[-3].val);
            std::shared_ptr<SQLExpression> vectorIndex = (eckit_sql_vsp[-2].exp);
            std::string tableReference((eckit_sql_vsp[-1].val)); // TODO: table_reference should handle .<database> suffix
            std::shared_ptr<SQLExpression> pshift((eckit_sql_vsp[0].exp));

            std::string bitfieldName;
            (eckit_sql_val.exp) = session->selectFactory().createColumn(columnName, bitfieldName, vectorIndex, tableReference /*TODO: handle .<database> */, pshift);
         }
#line 1747 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 54: /* column: IDENT bitfield_ref table_reference optional_hash  */
#line 516 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                            {

            std::string columnName = (eckit_sql_vsp[-3].val);
            std::string bitfieldName = (eckit_sql_vsp[-2].val);
            std::shared_ptr<SQLExpression> vectorIndex;
            std::string tableReference((eckit_sql_vsp[-1].val)); // TODO: table_reference should handle .<database> suffix
            std::shared_ptr<SQLExpression> pshift       ((eckit_sql_vsp[0].exp));

            (eckit_sql_val.exp) = session->selectFactory().createColumn(columnName, bitfieldName, vectorIndex, tableReference /*TODO: handle .<database> */, pshift);
         }
#line 1762 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 55: /* vector_index: '[' expression ']'  */
#line 528 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                     { (eckit_sql_val.exp) = (eckit_sql_vsp[-1].exp); }
#line 1768 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 56: /* vector_index: empty  */
#line 529 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                     { (eckit_sql_val.exp) = NULL; }
#line 1774 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 57: /* table_reference: '@' IDENT  */
#line 532 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                             { (eckit_sql_val.val) = std::string("@") + (eckit_sql_vsp[0].val); }
#line 1780 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 58: /* table_reference: empty  */
#line 533 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                             { (eckit_sql_val.val) = std::string(""); }
#line 1786 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 59: /* table: DUAL  */
#line 538 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                        { (eckit_sql_val.table) = 0; }
#line 1792 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 60: /* table: IDENT  */
#line 539 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                        { (eckit_sql_val.table) = &session->findTable((eckit_sql_vsp[0].val)); }
#line 1798 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 61: /* table: STRING  */
#line 540 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                { (eckit_sql_val.table) = &session->findTable((eckit_sql_vsp[0].val)); }
#line 1804 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 62: /* table_list: table  */
#line 543 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                    { (eckit_sql_val.tablist) = std::vector<std::reference_wrapper<SQLTable>>(); if((eckit_sql_vsp[0].table)) { (eckit_sql_val.tablist).push_back(*(eckit_sql_vsp[0].table)); } }
#line 1810 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 63: /* table_list: table_list ',' table  */
#line 544 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                    { (eckit_sql_val.tablist) = (eckit_sql_vsp[-2].tablist); if ((eckit_sql_vsp[0].table)) { (eckit_sql_val.tablist).push_back(*(eckit_sql_vsp[0].table)); } }
#line 1816 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 64: /* select_list: select_list_  */
#line 549 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                              { (eckit_sql_val.explist) = (eckit_sql_vsp[0].explist); }
#line 1822 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 65: /* select_list: select_list_ ','  */
#line 550 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                              { (eckit_sql_val.explist) = (eckit_sql_vsp[-1].explist); }
#line 1828 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 66: /* select_list_: select  */
#line 553 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                              { (eckit_sql_val.explist) = Expressions(); (eckit_sql_val.explist).push_back((eckit_sql_vsp[0].exp)); }
#line 1834 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 67: /* select_list_: select_list_ ',' select  */
#line 554 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                              { (eckit_sql_val.explist) = (eckit_sql_vsp[-2].explist); (eckit_sql_val.explist).push_back((eckit_sql_vsp[0].exp)); }
#line 1840 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 68: /* select: select_ access_decl  */
#line 557 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                            { (eckit_sql_val.exp) = (eckit_sql_vsp[-1].exp); }
#line 1846 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 69: /* select_: '*' table_reference  */
#line 560 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                         { (eckit_sql_val.exp) = std::make_shared<ColumnExpression>("*", (eckit_sql_vsp[0].val));  }
#line 1852 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 70: /* select_: IDENT '.' '*' table_reference  */
#line 561 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                         { (eckit_sql_val.exp) = std::make_shared<BitColumnExpression>((eckit_sql_vsp[-3].val), "*", (eckit_sql_vsp[0].val)); }
#line 1858 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 71: /* select_: expression AS IDENT table_reference  */
#line 571 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                             { (eckit_sql_val.exp) = (eckit_sql_vsp[-3].exp); (eckit_sql_val.exp)->title((eckit_sql_vsp[-1].val) + (eckit_sql_vsp[0].val)); }
#line 1864 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 76: /* group_by: GROUP BY expression_list  */
#line 582 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                   { (eckit_sql_val.explist) = (eckit_sql_vsp[0].explist);                       }
#line 1870 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 77: /* group_by: empty  */
#line 583 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                 { (eckit_sql_val.explist) = Expressions(); }
#line 1876 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 78: /* order_by: ORDER BY order_list  */
#line 588 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                              { (eckit_sql_val.orderlist) = (eckit_sql_vsp[0].orderlist);                       }
#line 1882 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 79: /* order_by: empty  */
#line 589 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                              { (eckit_sql_val.orderlist) = std::make_pair(Expressions(),std::vector<bool>()); }
#line 1888 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 80: /* order_list: order  */
#line 593 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                    { (eckit_sql_val.orderlist) = std::make_pair(Expressions(), std::vector<bool>()); (eckit_sql_val.orderlist).first.push_back((eckit_sql_vsp[0].orderexp).first); (eckit_sql_val.orderlist).second.push_back((eckit_sql_vsp[0].orderexp).second); }
#line 1894 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 81: /* order_list: order_list ',' order  */
#line 594 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                    { (eckit_sql_val.orderlist) = (eckit_sql_vsp[-2].orderlist); (eckit_sql_val.orderlist).first.push_back((eckit_sql_vsp[0].orderexp).first); (eckit_sql_val.orderlist).second.push_back((eckit_sql_vsp[0].orderexp).second); }
#line 1900 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 82: /* order: expression DESC  */
#line 597 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                             { (eckit_sql_val.orderexp) = std::make_pair((eckit_sql_vsp[-1].exp), false); }
#line 1906 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 83: /* order: expression ASC  */
#line 598 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                             { (eckit_sql_val.orderexp) = std::make_pair((eckit_sql_vsp[-1].exp), true); }
#line 1912 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 84: /* order: expression  */
#line 599 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                         { (eckit_sql_val.orderexp) = std::make_pair((eckit_sql_vsp[0].exp), true); }
#line 1918 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 85: /* expression_list: expression  */
#line 605 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                     {  (eckit_sql_val.explist) = Expressions(); (eckit_sql_val.explist).push_back((eckit_sql_vsp[0].exp)); }
#line 1924 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 86: /* expression_list: expression_list ',' expression  */
#line 606 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                 { (eckit_sql_val.explist) = (eckit_sql_vsp[-2].explist); (eckit_sql_val.explist).push_back((eckit_sql_vsp[0].exp)); }
#line 1930 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 87: /* optional_hash: HASH DOUBLE  */
#line 609 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                            { (eckit_sql_val.exp) = std::make_shared<NumberExpression>((eckit_sql_vsp[0].num)); }
#line 1936 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 88: /* optional_hash: %empty  */
#line 610 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                            { (eckit_sql_val.exp) = std::make_shared<NumberExpression>(0); }
#line 1942 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 89: /* atom_or_number: '(' expression ')'  */
#line 614 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = (eckit_sql_vsp[-1].exp); }
#line 1948 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 90: /* atom_or_number: '-' expression  */
#line 615 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = FunctionFactory::instance().build("-",(eckit_sql_vsp[0].exp)); }
#line 1954 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 91: /* atom_or_number: DOUBLE  */
#line 616 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = std::make_shared<NumberExpression>((eckit_sql_vsp[0].num)); }
#line 1960 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 93: /* atom_or_number: VAR  */
#line 619 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = session->currentDatabase().getVariable((eckit_sql_vsp[0].val)); }
#line 1966 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 94: /* atom_or_number: '?' DOUBLE  */
#line 620 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = std::make_shared<ParameterExpression>((eckit_sql_vsp[0].num)); }
#line 1972 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 95: /* atom_or_number: func '(' expression_list ')'  */
#line 621 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = FunctionFactory::instance().build((eckit_sql_vsp[-3].val), (eckit_sql_vsp[-1].explist)); }
#line 1978 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 96: /* atom_or_number: func '(' empty ')'  */
#line 622 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = FunctionFactory::instance().build((eckit_sql_vsp[-3].val), emptyExpressionList); }
#line 1984 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 97: /* atom_or_number: func '(' '*' ')'  */
#line 624 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                {
                    if (std::string("count") != (eckit_sql_vsp[-3].val))
                        throw eckit::UserError(std::string("Only function COUNT can accept '*' as parameter (") + (eckit_sql_vsp[-3].val) + ")");

                    (eckit_sql_val.exp) = FunctionFactory::instance().build("count", std::make_shared<NumberExpression>(1.0));
                }
#line 1995 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 98: /* atom_or_number: STRING  */
#line 630 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = std::make_shared<StringExpression>((eckit_sql_vsp[0].val)); }
#line 2001 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 99: /* func: IDENT  */
#line 634 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
             { (eckit_sql_val.val) = (eckit_sql_vsp[0].val);      }
#line 2007 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 100: /* func: COUNT  */
#line 635 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
             { (eckit_sql_val.val) = "count"; }
#line 2013 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 101: /* factor: factor '*' atom_or_number  */
#line 640 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                 { (eckit_sql_val.exp) = FunctionFactory::instance().build("*",(eckit_sql_vsp[-2].exp),(eckit_sql_vsp[0].exp));   }
#line 2019 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 102: /* factor: factor '/' atom_or_number  */
#line 641 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                 { (eckit_sql_val.exp) = FunctionFactory::instance().build("/",(eckit_sql_vsp[-2].exp),(eckit_sql_vsp[0].exp)); }
#line 2025 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 104: /* term: term '+' factor  */
#line 646 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                        { (eckit_sql_val.exp) = FunctionFactory::instance().build("+",(eckit_sql_vsp[-2].exp),(eckit_sql_vsp[0].exp));   }
#line 2031 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 105: /* term: term '-' factor  */
#line 647 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                        { (eckit_sql_val.exp) = FunctionFactory::instance().build("-",(eckit_sql_vsp[-2].exp),(eckit_sql_vsp[0].exp));   }
#line 2037 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 107: /* relational_operator: '>'  */
#line 652 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.val) = ">"; }
#line 2043 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 108: /* relational_operator: EQ  */
#line 653 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.val) = "="; }
#line 2049 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 109: /* relational_operator: '<'  */
#line 654 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.val) = "<"; }
#line 2055 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 110: /* relational_operator: GE  */
#line 655 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.val) = ">="; }
#line 2061 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 111: /* relational_operator: LE  */
#line 656 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.val) = "<="; }
#line 2067 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 112: /* relational_operator: NE  */
#line 657 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                         { (eckit_sql_val.val) = "<>"; }
#line 2073 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 113: /* condition: term relational_operator term relational_operator term  */
#line 660 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                                     { (eckit_sql_val.exp) = FunctionFactory::instance().build("and", FunctionFactory::instance().build((eckit_sql_vsp[-3].val),(eckit_sql_vsp[-4].exp),(eckit_sql_vsp[-2].exp)), FunctionFactory::instance().build((eckit_sql_vsp[-1].val),(eckit_sql_vsp[-2].exp),(eckit_sql_vsp[0].exp))); }
#line 2079 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 114: /* condition: term relational_operator term  */
#line 661 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                                     { (eckit_sql_val.exp) = FunctionFactory::instance().build((eckit_sql_vsp[-1].val), (eckit_sql_vsp[-2].exp), (eckit_sql_vsp[0].exp)); }
#line 2085 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 115: /* condition: MATCH '(' expression_list ')' IN QUERY '(' select_statement ')'  */
#line 663 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
            {
                 NOTIMP;
            //    const Expressions& matchList ($3);
            //    const SelectAST& subquery ($8);
            //    $$ = FunctionFactory::instance().build("match", matchList, subquery);
            }
#line 2096 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 116: /* condition: condition IN '(' expression_list ')'  */
#line 669 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                                     { (eckit_sql_vsp[-1].explist).push_back((eckit_sql_vsp[-4].exp)); (eckit_sql_val.exp) = FunctionFactory::instance().build("in",(eckit_sql_vsp[-1].explist));   }
#line 2102 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 117: /* condition: condition NOT IN '(' expression_list ')'  */
#line 670 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                         { (eckit_sql_vsp[-1].explist).push_back((eckit_sql_vsp[-5].exp)); (eckit_sql_val.exp) = FunctionFactory::instance().build("not_in",(eckit_sql_vsp[-1].explist));   }
#line 2108 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 118: /* condition: NOT condition  */
#line 671 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                        { (eckit_sql_val.exp) = FunctionFactory::instance().build("not",(eckit_sql_vsp[0].exp));   }
#line 2114 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 119: /* condition: condition IS NIL  */
#line 672 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                        { (eckit_sql_val.exp) = FunctionFactory::instance().build("null",(eckit_sql_vsp[-2].exp));   }
#line 2120 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 120: /* condition: condition IS NOT NIL  */
#line 673 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                        { (eckit_sql_val.exp) = FunctionFactory::instance().build("not_null",(eckit_sql_vsp[-3].exp));   }
#line 2126 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 121: /* condition: condition BETWEEN term AND term  */
#line 674 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = FunctionFactory::instance().build("between",(eckit_sql_vsp[-4].exp),(eckit_sql_vsp[-2].exp),(eckit_sql_vsp[0].exp)); }
#line 2132 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 122: /* condition: condition NOT BETWEEN term AND term  */
#line 675 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                                  { (eckit_sql_val.exp) = FunctionFactory::instance().build("not_between",(eckit_sql_vsp[-5].exp),(eckit_sql_vsp[-2].exp),(eckit_sql_vsp[0].exp)); }
#line 2138 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 123: /* condition: condition LIKE term  */
#line 676 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                        { (eckit_sql_val.exp) = FunctionFactory::instance().build("like", (eckit_sql_vsp[-2].exp), (eckit_sql_vsp[0].exp)); }
#line 2144 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 124: /* condition: condition RLIKE term  */
#line 677 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                        { (eckit_sql_val.exp) = FunctionFactory::instance().build("rlike", (eckit_sql_vsp[-2].exp), (eckit_sql_vsp[0].exp)); }
#line 2150 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 126: /* conjonction: conjonction AND condition  */
#line 681 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = FunctionFactory::instance().build("and",(eckit_sql_vsp[-2].exp),(eckit_sql_vsp[0].exp));   }
#line 2156 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 128: /* disjonction: disjonction OR conjonction  */
#line 685 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
                                              { (eckit_sql_val.exp) = FunctionFactory::instance().build("or",(eckit_sql_vsp[-2].exp),(eckit_sql_vsp[0].exp));   }
#line 2162 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;

  case 131: /* expression: expression '[' expression ']'  */
#line 692 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"
            {
                // This has not been implemented yet.
                // SDS: n.b. seems to be in old versions. Not sure why deprecated
                throw UserError("Syntax: 'expression [ expression ]' not yet supported");
            }
#line 2172 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"
    break;


#line 2176 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/sql/sqly.c"

      default: break;
    }
  /* User semantic actions sometimes alter eckit_sql_char, and that requires
     that eckit_sql_token be updated with the new translation.  We take the
     approach of translating immediately before every use of eckit_sql_token.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering eckit_sql_char or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (eckit_sql_symbol_kind_t, eckit_sql_r1[eckit_sql_n]), &eckit_sql_val, &eckit_sql_loc);

  YYPOPSTACK (eckit_sql_len);
  eckit_sql_len = 0;

  *++eckit_sql_vsp = eckit_sql_val;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int eckit_sql_lhs = eckit_sql_r1[eckit_sql_n] - YYNTOKENS;
    const int eckit_sql_i = eckit_sql_pgoto[eckit_sql_lhs] + *eckit_sql_ssp;
    eckit_sql_state = (0 <= eckit_sql_i && eckit_sql_i <= YYLAST && eckit_sql_check[eckit_sql_i] == *eckit_sql_ssp
               ? eckit_sql_table[eckit_sql_i]
               : eckit_sql_defgoto[eckit_sql_lhs]);
  }

  goto eckit_sql_newstate;


/*--------------------------------------.
| eckit_sql_errlab -- here on detecting error.  |
`--------------------------------------*/
eckit_sql_errlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  eckit_sql_token = eckit_sql_char == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (eckit_sql_char);
  /* If not already recovering from an error, report this error.  */
  if (!eckit_sql_errstatus)
    {
      ++eckit_sql_nerrs;
      eckit_sql_error (scanner, session, YY_("syntax error"));
    }

  if (eckit_sql_errstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (eckit_sql_char <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (eckit_sql_char == YYEOF)
            YYABORT;
        }
      else
        {
          eckit_sql_destruct ("Error: discarding",
                      eckit_sql_token, &eckit_sql_lval, scanner, session);
          eckit_sql_char = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto eckit_sql_errlab1;


/*---------------------------------------------------.
| eckit_sql_errorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
eckit_sql_errorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label eckit_sql_errorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++eckit_sql_nerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (eckit_sql_len);
  eckit_sql_len = 0;
  YY_STACK_PRINT (eckit_sql_ss, eckit_sql_ssp);
  eckit_sql_state = *eckit_sql_ssp;
  goto eckit_sql_errlab1;


/*-------------------------------------------------------------.
| eckit_sql_errlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
eckit_sql_errlab1:
  eckit_sql_errstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      eckit_sql_n = eckit_sql_pact[eckit_sql_state];
      if (!eckit_sql_pact_value_is_default (eckit_sql_n))
        {
          eckit_sql_n += YYSYMBOL_YYerror;
          if (0 <= eckit_sql_n && eckit_sql_n <= YYLAST && eckit_sql_check[eckit_sql_n] == YYSYMBOL_YYerror)
            {
              eckit_sql_n = eckit_sql_table[eckit_sql_n];
              if (0 < eckit_sql_n)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (eckit_sql_ssp == eckit_sql_ss)
        YYABORT;


      eckit_sql_destruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (eckit_sql_state), eckit_sql_vsp, scanner, session);
      YYPOPSTACK (1);
      eckit_sql_state = *eckit_sql_ssp;
      YY_STACK_PRINT (eckit_sql_ss, eckit_sql_ssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++eckit_sql_vsp = eckit_sql_lval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (eckit_sql_n), eckit_sql_vsp, eckit_sql_lsp);

  eckit_sql_state = eckit_sql_n;
  goto eckit_sql_newstate;


/*-------------------------------------.
| eckit_sql_acceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
eckit_sql_acceptlab:
  eckit_sql_result = 0;
  goto eckit_sql_returnlab;


/*-----------------------------------.
| eckit_sql_abortlab -- YYABORT comes here.  |
`-----------------------------------*/
eckit_sql_abortlab:
  eckit_sql_result = 1;
  goto eckit_sql_returnlab;


/*-----------------------------------------------------------.
| eckit_sql_exhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
eckit_sql_exhaustedlab:
  eckit_sql_error (scanner, session, YY_("memory exhausted"));
  eckit_sql_result = 2;
  goto eckit_sql_returnlab;


/*----------------------------------------------------------.
| eckit_sql_returnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
eckit_sql_returnlab:
  if (eckit_sql_char != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      eckit_sql_token = YYTRANSLATE (eckit_sql_char);
      eckit_sql_destruct ("Cleanup: discarding lookahead",
                  eckit_sql_token, &eckit_sql_lval, scanner, session);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (eckit_sql_len);
  YY_STACK_PRINT (eckit_sql_ss, eckit_sql_ssp);
  while (eckit_sql_ssp != eckit_sql_ss)
    {
      eckit_sql_destruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*eckit_sql_ssp), eckit_sql_vsp, scanner, session);
      YYPOPSTACK (1);
    }
#ifndef eckit_sql_overflow
  if (eckit_sql_ss != eckit_sql_ssa)
    YYSTACK_FREE (eckit_sql_ss);
#endif

  return eckit_sql_result;
}

#line 702 "/home/runner/work/eckit/eckit/src/eckit/sql/sqly.y"

#undef YY_NULL
#include "sqll.c"

