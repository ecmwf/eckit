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
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
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
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"


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


#line 110 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"

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

#include "cmdsy.tmp.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_STRING = 3,                     /* STRING  */
  YYSYMBOL_OPTION = 4,                     /* OPTION  */
  YYSYMBOL_SHELL = 5,                      /* SHELL  */
  YYSYMBOL_PIPE = 6,                       /* PIPE  */
  YYSYMBOL_OUTPUT = 7,                     /* OUTPUT  */
  YYSYMBOL_OUTAPPEND = 8,                  /* OUTAPPEND  */
  YYSYMBOL_NUMB = 9,                       /* NUMB  */
  YYSYMBOL_DAY = 10,                       /* DAY  */
  YYSYMBOL_MONTH = 11,                     /* MONTH  */
  YYSYMBOL_WEEK = 12,                      /* WEEK  */
  YYSYMBOL_HOUR = 13,                      /* HOUR  */
  YYSYMBOL_MINUTE = 14,                    /* MINUTE  */
  YYSYMBOL_SECOND = 15,                    /* SECOND  */
  YYSYMBOL_YEAR = 16,                      /* YEAR  */
  YYSYMBOL_AND = 17,                       /* AND  */
  YYSYMBOL_OR = 18,                        /* OR  */
  YYSYMBOL_LE = 19,                        /* LE  */
  YYSYMBOL_GE = 20,                        /* GE  */
  YYSYMBOL_NE = 21,                        /* NE  */
  YYSYMBOL_NOT = 22,                       /* NOT  */
  YYSYMBOL_IN = 23,                        /* IN  */
  YYSYMBOL_MATCH = 24,                     /* MATCH  */
  YYSYMBOL_HEXABYTE = 25,                  /* HEXABYTE  */
  YYSYMBOL_PETABYTE = 26,                  /* PETABYTE  */
  YYSYMBOL_TERABYTE = 27,                  /* TERABYTE  */
  YYSYMBOL_GIGABYTE = 28,                  /* GIGABYTE  */
  YYSYMBOL_MEGABYTE = 29,                  /* MEGABYTE  */
  YYSYMBOL_KILOBYTE = 30,                  /* KILOBYTE  */
  YYSYMBOL_BYTE = 31,                      /* BYTE  */
  YYSYMBOL_32_ = 32,                       /* ';'  */
  YYSYMBOL_33_ = 33,                       /* '@'  */
  YYSYMBOL_34_ = 34,                       /* '='  */
  YYSYMBOL_35_ = 35,                       /* '$'  */
  YYSYMBOL_36_ = 36,                       /* '['  */
  YYSYMBOL_37_ = 37,                       /* ']'  */
  YYSYMBOL_38_ = 38,                       /* '-'  */
  YYSYMBOL_39_ = 39,                       /* '('  */
  YYSYMBOL_40_ = 40,                       /* ')'  */
  YYSYMBOL_41_ = 41,                       /* '^'  */
  YYSYMBOL_42_ = 42,                       /* '*'  */
  YYSYMBOL_43_ = 43,                       /* '/'  */
  YYSYMBOL_44_ = 44,                       /* '%'  */
  YYSYMBOL_45_ = 45,                       /* '+'  */
  YYSYMBOL_46_ = 46,                       /* '&'  */
  YYSYMBOL_47_ = 47,                       /* '>'  */
  YYSYMBOL_48_ = 48,                       /* '<'  */
  YYSYMBOL_49_ = 49,                       /* ','  */
  YYSYMBOL_YYACCEPT = 50,                  /* $accept  */
  YYSYMBOL_start = 51,                     /* start  */
  YYSYMBOL_lines = 52,                     /* lines  */
  YYSYMBOL_line = 53,                      /* line  */
  YYSYMBOL_eol = 54,                       /* eol  */
  YYSYMBOL_command = 55,                   /* command  */
  YYSYMBOL_args = 56,                      /* args  */
  YYSYMBOL_arg = 57,                       /* arg  */
  YYSYMBOL_variable = 58,                  /* variable  */
  YYSYMBOL_params = 59,                    /* params  */
  YYSYMBOL_param = 60,                     /* param  */
  YYSYMBOL_value = 61,                     /* value  */
  YYSYMBOL_power = 62,                     /* power  */
  YYSYMBOL_factor = 63,                    /* factor  */
  YYSYMBOL_term = 64,                      /* term  */
  YYSYMBOL_condition = 65,                 /* condition  */
  YYSYMBOL_conjonction = 66,               /* conjonction  */
  YYSYMBOL_disjonction = 67,               /* disjonction  */
  YYSYMBOL_expression = 68,                /* expression  */
  YYSYMBOL_list = 69,                      /* list  */
  YYSYMBOL_number = 70,                    /* number  */
  YYSYMBOL_ages = 71,                      /* ages  */
  YYSYMBOL_age = 72,                       /* age  */
  YYSYMBOL_sizes = 73,                     /* sizes  */
  YYSYMBOL_size = 74,                      /* size  */
  YYSYMBOL_empty = 75                      /* empty  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




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
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
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
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
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
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
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

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
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
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  22
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   141

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  85
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  120

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   286


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    35,    44,    46,     2,
      39,    40,    42,    45,    49,    38,     2,    43,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    32,
      48,    34,    47,     2,    33,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    36,     2,    37,    41,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    92,    92,    93,    96,    97,   100,   101,   102,   105,
     106,   109,   113,   117,   122,   127,   132,   133,   136,   137,
     138,   139,   140,   143,   144,   147,   150,   151,   152,   155,
     158,   159,   160,   161,   162,   163,   164,   169,   170,   173,
     174,   175,   176,   179,   180,   181,   182,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   197,   198,   201,
     202,   205,   209,   210,   213,   214,   215,   218,   219,   222,
     223,   224,   225,   226,   227,   228,   231,   232,   235,   236,
     237,   238,   239,   240,   241,   244
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "STRING", "OPTION",
  "SHELL", "PIPE", "OUTPUT", "OUTAPPEND", "NUMB", "DAY", "MONTH", "WEEK",
  "HOUR", "MINUTE", "SECOND", "YEAR", "AND", "OR", "LE", "GE", "NE", "NOT",
  "IN", "MATCH", "HEXABYTE", "PETABYTE", "TERABYTE", "GIGABYTE",
  "MEGABYTE", "KILOBYTE", "BYTE", "';'", "'@'", "'='", "'$'", "'['", "']'",
  "'-'", "'('", "')'", "'^'", "'*'", "'/'", "'%'", "'+'", "'&'", "'>'",
  "'<'", "','", "$accept", "start", "lines", "line", "eol", "command",
  "args", "arg", "variable", "params", "param", "value", "power", "factor",
  "term", "condition", "conjonction", "disjonction", "expression", "list",
  "number", "ages", "age", "sizes", "size", "empty", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-62)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-86)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      53,   -25,    10,   -62,   -62,     5,    26,    66,   -62,   -62,
     -25,    33,   -62,   -62,    25,    25,    22,     3,    11,   -62,
     -62,    11,   -62,   -62,   -62,     1,    79,    13,    46,    21,
     -62,   -62,    56,   -62,    63,   -62,   -62,   -62,   -62,   -62,
     -62,    25,   -62,   -62,   -62,    25,    25,    47,   -62,   -62,
     -62,   -62,   -62,   -62,   -62,   -62,   -62,   -62,   -62,   -62,
     -62,   -62,   -62,   -62,   -20,   -62,    21,    55,   -62,    -7,
      38,    54,    45,    82,    57,   109,   -62,   101,   -62,   -62,
     -62,   -62,    25,    54,    25,    25,    25,    25,    25,    25,
      25,    25,    25,    25,    25,    25,    25,    25,    25,    21,
      21,   -62,   -62,   -62,   -62,   -62,   -62,    -7,    -7,    -7,
      38,    38,    38,    38,    38,    38,    38,    38,    54,    45
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    85,    85,    17,     9,     0,     0,     0,     5,     7,
      85,    10,     8,    10,    24,     0,     0,    11,    20,    22,
      18,    85,     1,     4,     6,    34,    66,     0,     0,     0,
      23,    32,    64,    68,    65,    77,    16,    25,    13,    14,
      15,    21,    27,    29,    28,    19,    12,     0,    72,    74,
      73,    71,    70,    69,    75,    84,    83,    82,    81,    80,
      79,    78,    31,    63,     0,    33,     0,    38,    42,    46,
      56,    58,    60,    61,     0,     0,    67,     0,    76,    26,
      36,    30,     0,    55,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    35,    62,    37,    39,    40,    41,    44,    43,    45,
      51,    50,    52,    53,    54,    48,    47,    49,    57,    59
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -62,   -62,   -62,    96,    69,   -62,     0,   -62,   -62,   -62,
      97,   -14,    49,    -8,    20,   -61,    37,   -62,   -62,   -62,
     111,   -62,   108,   -62,   107,     2
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     6,     7,     8,     9,    10,    17,    18,    19,    41,
      42,    67,    68,    69,    70,    71,    72,    73,    74,    64,
      31,    32,    33,    34,    35,    13
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      30,    36,    11,    43,    20,    83,    25,     4,    21,    38,
      39,    40,    26,    63,    14,    14,    25,    81,    45,    44,
      20,    46,    26,    20,    25,    37,    22,    43,    25,    82,
      26,    43,    43,    -3,    26,    85,    86,    87,   118,    27,
      47,    28,    29,    66,    15,    16,    16,    44,    44,    27,
      62,    28,    29,   -85,     1,    26,     2,    27,     3,    28,
      29,    27,    99,    28,    29,    75,    -2,     1,   102,     2,
      12,     3,    77,    91,    92,    93,    88,    94,    95,    24,
     107,   108,   109,    89,    90,     4,     5,    80,    96,    48,
      49,    50,    51,    52,    53,    54,    84,   101,     4,     5,
     100,    97,    98,    23,    55,    56,    57,    58,    59,    60,
      61,   110,   111,   112,   113,   114,   115,   116,   117,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,   103,   104,   105,   106,   119,    79,    65,
      76,    78
};

static const yytype_int8 yycheck[] =
{
      14,    15,     0,    17,     2,    66,     3,    32,     3,     6,
       7,     8,     9,    27,     4,     4,     3,    37,    18,    17,
      18,    21,     9,    21,     3,     3,     0,    41,     3,    49,
       9,    45,    46,     0,     9,    42,    43,    44,    99,    36,
      39,    38,    39,    22,    34,    35,    35,    45,    46,    36,
      37,    38,    39,     0,     1,     9,     3,    36,     5,    38,
      39,    36,    17,    38,    39,     9,     0,     1,    82,     3,
       1,     5,     9,    19,    20,    21,    38,    23,    24,    10,
      88,    89,    90,    45,    46,    32,    33,    40,    34,    10,
      11,    12,    13,    14,    15,    16,    41,    40,    32,    33,
      18,    47,    48,     7,    25,    26,    27,    28,    29,    30,
      31,    91,    92,    93,    94,    95,    96,    97,    98,    10,
      11,    12,    13,    14,    15,    16,    25,    26,    27,    28,
      29,    30,    31,    84,    85,    86,    87,   100,    41,    28,
      32,    34
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     5,    32,    33,    51,    52,    53,    54,
      55,    75,    54,    75,     4,    34,    35,    56,    57,    58,
      75,     3,     0,    53,    54,     3,     9,    36,    38,    39,
      61,    70,    71,    72,    73,    74,    61,     3,     6,     7,
       8,    59,    60,    61,    75,    56,    56,    39,    10,    11,
      12,    13,    14,    15,    16,    25,    26,    27,    28,    29,
      30,    31,    37,    61,    69,    70,    22,    61,    62,    63,
      64,    65,    66,    67,    68,     9,    72,     9,    74,    60,
      40,    37,    49,    65,    41,    42,    43,    44,    38,    45,
      46,    19,    20,    21,    23,    24,    34,    47,    48,    17,
      18,    40,    61,    62,    62,    62,    62,    63,    63,    63,
      64,    64,    64,    64,    64,    64,    64,    64,    65,    66
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    50,    51,    51,    52,    52,    53,    53,    53,    54,
      54,    55,    55,    55,    55,    55,    55,    55,    56,    56,
      56,    56,    56,    57,    57,    58,    59,    59,    59,    60,
      61,    61,    61,    61,    61,    61,    61,    62,    62,    63,
      63,    63,    63,    64,    64,    64,    64,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    66,    66,    67,
      67,    68,    69,    69,    70,    70,    70,    71,    71,    72,
      72,    72,    72,    72,    72,    72,    73,    73,    74,    74,
      74,    74,    74,    74,    74,    75
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     2,     1,     2,     1,
       1,     2,     3,     3,     3,     3,     3,     1,     1,     2,
       1,     2,     1,     2,     1,     2,     2,     1,     1,     1,
       3,     2,     1,     2,     1,     3,     3,     3,     1,     3,
       3,     3,     1,     3,     3,     3,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     1,     3,     1,     3,
       1,     1,     3,     1,     1,     1,     1,     2,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     2,     2,
       2,     2,     2,     2,     2,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
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
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
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
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 9: /* eol: ';'  */
#line 105 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                                        { CmdParser::reset(); }
#line 1258 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 11: /* command: STRING args  */
#line 109 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                {
								  CmdParser::arg(0,(yyvsp[-1].str));
								  CmdParser::run(CmdResource::command);
								}
#line 1267 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 12: /* command: '@' STRING args  */
#line 113 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                {
								  CmdParser::arg(0,(yyvsp[-1].str));
								  CmdParser::run(CmdResource::loop);
								}
#line 1276 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 13: /* command: STRING args PIPE  */
#line 117 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                {
								  CmdParser::arg("|",(yyvsp[0].str));
								  CmdParser::arg(0,(yyvsp[-2].str));
								  CmdParser::run(CmdResource::pipe);
								}
#line 1286 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 14: /* command: STRING args OUTPUT  */
#line 122 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                {
								  CmdParser::arg(">",(yyvsp[0].str));
								  CmdParser::arg(0,(yyvsp[-2].str));
								  CmdParser::run(CmdResource::redirect);
								}
#line 1296 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 15: /* command: STRING args OUTAPPEND  */
#line 127 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                {
								  CmdParser::arg(">>",(yyvsp[0].str));
								  CmdParser::arg(0,(yyvsp[-2].str));
								  CmdParser::run(CmdResource::append);
								}
#line 1306 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 16: /* command: STRING '=' value  */
#line 132 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                         { CmdParser::var((yyvsp[-2].str),(yyvsp[0].val)); }
#line 1312 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 17: /* command: SHELL  */
#line 133 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                         { CmdParser::shell((yyvsp[0].str));    }
#line 1318 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 23: /* arg: OPTION value  */
#line 143 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                        { CmdParser::arg((yyvsp[-1].str),(yyvsp[0].val)); }
#line 1324 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 24: /* arg: OPTION  */
#line 144 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                        { CmdParser::arg((yyvsp[0].str),std::string("1")); }
#line 1330 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 25: /* variable: '$' STRING  */
#line 147 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                        { CmdParser::var((yyvsp[0].str)); }
#line 1336 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 29: /* param: value  */
#line 155 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
              {  CmdParser::arg((yyvsp[0].val)); }
#line 1342 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 30: /* value: '[' list ']'  */
#line 158 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                           { (yyval.val) = (yyvsp[-1].val); }
#line 1348 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 31: /* value: '[' ']'  */
#line 159 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                               { (yyval.val) = Value::makeList(); }
#line 1354 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 32: /* value: number  */
#line 160 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                               { (yyval.val) = Value((yyvsp[0].num)); }
#line 1360 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 33: /* value: '-' number  */
#line 161 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                               { (yyval.val) = Function("neg",(yyvsp[0].num)); }
#line 1366 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 34: /* value: STRING  */
#line 162 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                               { (yyval.val) = Value((yyvsp[0].str)); }
#line 1372 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 35: /* value: '(' expression ')'  */
#line 163 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                               { (yyval.val) = (yyvsp[-1].val); }
#line 1378 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 36: /* value: STRING '(' ')'  */
#line 164 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                               { (yyval.val) = Value((yyvsp[-2].str) + "()"); }
#line 1384 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 37: /* power: value '^' power  */
#line 169 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                      { (yyval.val) = Function("pow",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1390 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 39: /* factor: factor '*' power  */
#line 173 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                     { (yyval.val) = Function("mul",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1396 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 40: /* factor: factor '/' power  */
#line 174 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                             { (yyval.val) = Function("div",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1402 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 41: /* factor: factor '%' power  */
#line 175 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                             { (yyval.val) = Function("mod",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1408 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 43: /* term: term '+' factor  */
#line 179 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                     { (yyval.val) = Function("add",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1414 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 44: /* term: term '-' factor  */
#line 180 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                             { (yyval.val) = Function("sub",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1420 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 45: /* term: term '&' factor  */
#line 181 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                             { (yyval.val) = Function("cat",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1426 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 47: /* condition: condition '>' term  */
#line 185 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                         { (yyval.val) = Function("gt",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1432 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 48: /* condition: condition '=' term  */
#line 186 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                                 { (yyval.val) = Function("eq",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1438 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 49: /* condition: condition '<' term  */
#line 187 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                                 { (yyval.val) = Function("lt",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1444 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 50: /* condition: condition GE term  */
#line 188 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                                 { (yyval.val) = Function("ge",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1450 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 51: /* condition: condition LE term  */
#line 189 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                                 { (yyval.val) = Function("le",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1456 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 52: /* condition: condition NE term  */
#line 190 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                                 { (yyval.val) = Function("ne",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1462 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 53: /* condition: condition IN term  */
#line 191 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                                 { (yyval.val) = Function("in",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1468 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 54: /* condition: condition MATCH term  */
#line 192 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                                 { (yyval.val) = Function("match",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1474 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 55: /* condition: NOT condition  */
#line 193 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                                 { (yyval.val) = Function("not",(yyvsp[0].val)); }
#line 1480 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 57: /* conjonction: conjonction AND condition  */
#line 197 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                        { (yyval.val) = Function("and",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1486 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 59: /* disjonction: disjonction OR conjonction  */
#line 201 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                             { (yyval.val) = Function("or",(yyvsp[-2].val),(yyvsp[0].val)); }
#line 1492 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 62: /* list: list ',' value  */
#line 209 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                         { (yyval.val) = (yyvsp[-2].val) + Value::makeList((yyvsp[0].val)); }
#line 1498 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 63: /* list: value  */
#line 210 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                             { (yyval.val) = Value::makeList((yyvsp[0].val)); }
#line 1504 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 67: /* ages: ages age  */
#line 218 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                     { (yyval.num) = (yyvsp[-1].num) + (yyvsp[0].num); }
#line 1510 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 69: /* age: NUMB SECOND  */
#line 222 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                        { (yyval.num) = (yyvsp[-1].num);}
#line 1516 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 70: /* age: NUMB MINUTE  */
#line 223 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                { (yyval.num) = (yyvsp[-1].num) * 60;}
#line 1522 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 71: /* age: NUMB HOUR  */
#line 224 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                { (yyval.num) = (yyvsp[-1].num) * 60 * 60;}
#line 1528 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 72: /* age: NUMB DAY  */
#line 225 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                { (yyval.num) = (yyvsp[-1].num) * 60 * 60 * 24;}
#line 1534 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 73: /* age: NUMB WEEK  */
#line 226 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                { (yyval.num) = (yyvsp[-1].num) * 60 * 60 * 24 * 7;}
#line 1540 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 74: /* age: NUMB MONTH  */
#line 227 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                { (yyval.num) = (yyvsp[-1].num) * 60 * 60 * 24 * 31;}
#line 1546 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 75: /* age: NUMB YEAR  */
#line 228 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                { (yyval.num) = (yyvsp[-1].num) * 60 * 60 * 24 * 365;}
#line 1552 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 76: /* sizes: sizes size  */
#line 231 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                       { (yyval.num) = (yyvsp[-1].num) + (yyvsp[0].num); }
#line 1558 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 78: /* size: NUMB BYTE  */
#line 235 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                           { (yyval.num) = (yyvsp[-1].num);}
#line 1564 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 79: /* size: NUMB KILOBYTE  */
#line 236 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                   { (yyval.num) = (yyvsp[-1].num) * 1024; }
#line 1570 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 80: /* size: NUMB MEGABYTE  */
#line 237 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                   { (yyval.num) = (yyvsp[-1].num) * 1024 * 1024; }
#line 1576 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 81: /* size: NUMB GIGABYTE  */
#line 238 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                   { (yyval.num) = (yyvsp[-1].num) * 1024 * 1024 * 1024; }
#line 1582 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 82: /* size: NUMB TERABYTE  */
#line 239 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                   { (yyval.num) = (yyvsp[-1].num) * 1024 * 1024 * 1024 * 1024; }
#line 1588 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 83: /* size: NUMB PETABYTE  */
#line 240 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                   { (yyval.num) = (yyvsp[-1].num) * 1024 * 1024 * 1024 * 1024 * 1024; }
#line 1594 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;

  case 84: /* size: NUMB HEXABYTE  */
#line 241 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"
                                   { (yyval.num) = (yyvsp[-1].num) * 1024 * 1024 * 1024 * 1024 * 1024 * 1024; }
#line 1600 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"
    break;


#line 1604 "/home/runner/work/eckit/eckit/_codeql_build_dir/src/eckit/cmd/cmdsy.tmp.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 247 "/home/runner/work/eckit/eckit/src/eckit/cmd/cmdsy.y"

#undef YY_NULL
#include "cmdsl.c"
