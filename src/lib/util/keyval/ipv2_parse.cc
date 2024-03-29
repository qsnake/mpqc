/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_KEYWORD_LEFT = 258,
     T_ARRAY_LEFT = 259,
     T_TABLE_LEFT = 260,
     T_KEYWORD_RIGHT = 261,
     T_ARRAY_RIGHT = 262,
     T_TABLE_RIGHT = 263,
     T_CONCAT = 264,
     T_STRING = 265,
     T_QUOTED_STRING = 266
   };
#endif
#define T_KEYWORD_LEFT 258
#define T_ARRAY_LEFT 259
#define T_TABLE_LEFT 260
#define T_KEYWORD_RIGHT 261
#define T_ARRAY_RIGHT 262
#define T_TABLE_RIGHT 263
#define T_CONCAT 264
#define T_STRING 265
#define T_QUOTED_STRING 266




/* Copy the first part of user declarations.  */
#line 27 "ipv2_parse.yy"

#ifdef DEC
#include <math.h>
#else
#include <stdlib.h>
#endif
#include <string.h>
#ifdef BISON
#define YYDEBUG 0
#if YYDEBUG != 0
int yydebug =1;
#endif /* YYDEBUG != 0 */
#endif /* BISON */
#if defined(SABER)
#define xmalloc malloc
#endif
#if defined(SGI)
#include <alloca.h>
#endif
#include <util/keyval/ipv2.h>
#define yyerror sc::IPV2::yerror
#define yyparse sc::IPV2::yparse
#define yylex sc::IPV2::ylex
#define yywrap sc::IPV2::ywrap


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 53 "ipv2_parse.yy"
typedef union YYSTYPE {
  char *str;
  sc::ip_string_list_t *sl;
  double dbl;
  } YYSTYPE;
/* Line 191 of yacc.c.  */
#line 129 "ipv2_parse.tmp.cc"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 141 "ipv2_parse.tmp.cc"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   136

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  22
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  30
/* YYNRULES -- Number of rules. */
#define YYNRULES  62
/* YYNRULES -- Number of states. */
#define YYNSTATES  102

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   266

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    21,     2,     2,     2,
       2,     2,    17,    19,    16,    18,     2,    20,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    12,     2,
      14,    13,    15,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    15,    19,    23,    27,
      28,    37,    43,    47,    51,    55,    58,    60,    64,    66,
      69,    70,    74,    75,    79,    80,    82,    86,    88,    90,
      92,    94,    97,   100,   104,   108,   110,   114,   120,   124,
     126,   128,   130,   132,   134,   138,   142,   146,   150,   154,
     156,   158,   161,   165,   168,   170,   174,   175,   179,   180,
     184,   186,   188
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      23,     0,    -1,    24,    -1,    24,    25,    -1,    -1,    36,
      12,     3,    24,     6,    -1,    36,    12,    30,    -1,    36,
      12,    25,    -1,    36,    13,    41,    -1,    -1,     5,    27,
       8,    26,    13,     5,    29,     8,    -1,    37,    12,     3,
      24,     6,    -1,    37,    12,    30,    -1,    37,    12,    25,
      -1,    37,    13,    41,    -1,    27,    28,    -1,    28,    -1,
      28,    12,    51,    -1,    51,    -1,    29,    41,    -1,    -1,
      34,    31,    35,    -1,    -1,    31,    32,    33,    -1,    -1,
      30,    -1,     3,    24,     6,    -1,    25,    -1,     4,    -1,
       7,    -1,    51,    -1,    51,    38,    -1,    51,    39,    -1,
      51,    39,    38,    -1,    51,    38,    39,    -1,    38,    -1,
      14,    51,    15,    -1,    14,    14,    40,    15,    15,    -1,
      40,    16,    51,    -1,    51,    -1,    47,    -1,    51,    -1,
      45,    -1,    42,    -1,     3,    43,     6,    -1,    44,    17,
      44,    -1,    44,    18,    44,    -1,    44,    19,    44,    -1,
      44,    20,    44,    -1,    45,    -1,    51,    -1,    21,    46,
      -1,    46,    12,    51,    -1,    12,    51,    -1,    51,    -1,
      34,    48,    35,    -1,    -1,    48,    49,    41,    -1,    -1,
      50,     9,    11,    -1,    11,    -1,    10,    -1,    50,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned char yyrline[] =
{
       0,    71,    71,    74,    75,    78,    80,    82,    84,    87,
      86,    91,    92,    93,    94,   108,   110,   115,   116,   119,
     120,   123,   126,   126,   127,   130,   131,   132,   135,   138,
     148,   149,   150,   151,   152,   155,   158,   161,   164,   165,
     168,   169,   171,   172,   175,   179,   180,   181,   182,   186,
     187,   190,   194,   195,   196,   199,   202,   202,   204,   207,
     214,   218,   219
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_KEYWORD_LEFT", "T_ARRAY_LEFT", 
  "T_TABLE_LEFT", "T_KEYWORD_RIGHT", "T_ARRAY_RIGHT", "T_TABLE_RIGHT", 
  "T_CONCAT", "T_STRING", "T_QUOTED_STRING", "':'", "'='", "'<'", "'>'", 
  "','", "'*'", "'-'", "'+'", "'/'", "'$'", "$accept", "input", 
  "group_defs", "group_def", "@1", "stringlist", "table_key", 
  "tablevalues", "karray_defs", "karray_elems", "@2", "karray_elem", 
  "array_left", "array_right", "keyword", "implicit_keyword", "polymorph", 
  "parentlist", "commalist", "value", "expression", "subexpression", 
  "expvalue", "var_sub", "var_key", "array", "values", "@3", 
  "quoted_string", "string", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,    58,    61,    60,    62,    44,    42,    45,    43,
      47,    36
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    22,    23,    24,    24,    25,    25,    25,    25,    26,
      25,    25,    25,    25,    25,    27,    27,    28,    28,    29,
      29,    30,    32,    31,    31,    33,    33,    33,    34,    35,
      36,    36,    36,    36,    36,    37,    38,    39,    40,    40,
      41,    41,    41,    41,    42,    43,    43,    43,    43,    44,
      44,    45,    46,    46,    46,    47,    49,    48,    48,    50,
      50,    51,    51
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     0,     5,     3,     3,     3,     0,
       8,     5,     3,     3,     3,     2,     1,     3,     1,     2,
       0,     3,     0,     3,     0,     1,     3,     1,     1,     1,
       1,     2,     2,     3,     3,     1,     3,     5,     3,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     1,
       1,     2,     3,     2,     1,     3,     0,     3,     0,     3,
       1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       4,     0,     2,     1,     0,    61,    60,     0,     3,     0,
       0,    35,    62,    30,     0,    16,    18,     0,     0,     0,
       0,     0,     0,     0,    31,    32,     9,    15,     0,    36,
       4,    28,     7,     6,    24,     0,     0,    58,     8,    43,
      42,    40,    41,     4,    13,    12,    14,    59,     0,     0,
      34,    33,     0,    17,     0,    22,     0,     0,    49,    50,
       0,    51,    54,    56,     0,     0,    39,     0,     5,    29,
       0,    21,    44,     0,     0,     0,     0,    53,     0,    55,
       0,    11,     0,     0,    20,     4,    27,    25,    23,    45,
      46,    47,    48,    52,    57,    37,    38,     0,     0,    10,
      19,    26
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     1,     2,     8,    52,    14,    15,    97,    33,    55,
      70,    88,    37,    71,     9,    10,    11,    25,    65,    38,
      39,    56,    57,    58,    61,    41,    63,    80,    12,    13
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -24
static const yysigned_char yypact[] =
{
     -24,    12,   113,   -24,    80,   -24,   -24,    80,   -24,   119,
     121,   -24,     5,     2,   118,     6,   -24,     8,    32,    30,
      78,    30,    16,    43,    24,    31,   -24,     6,    80,   -24,
     -24,   -24,   -24,   -24,   -24,    38,   110,   -24,   -24,   -24,
     -24,   -24,   -24,   -24,   -24,   -24,   -24,   -24,    80,    33,
     -24,   -24,    17,   -24,    93,    45,    44,    47,   -24,   -24,
      80,    56,   -24,    45,   103,   120,   -24,    68,   -24,   -24,
      91,   -24,   -24,    38,    38,    38,    38,   -24,    80,   -24,
      30,   -24,    60,    80,   -24,   -24,   -24,   -24,   -24,   -24,
     -24,   -24,   -24,   -24,   -24,   -24,   -24,    18,   105,   -24,
     -24,   -24
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -24,   -24,   -23,   -12,   -24,   -24,    64,   -24,   -15,   -24,
     -24,   -24,    -9,    34,   -24,   -24,     0,    76,   -24,   -20,
     -24,   -24,    11,   -17,   -24,   -24,   -24,   -24,   -24,    -4
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      16,    46,    40,    17,    40,    45,    32,    54,    44,    34,
      16,    34,     3,    24,    22,    42,    23,    42,    28,    17,
      64,    35,    31,    29,    53,    51,    99,    47,     5,     6,
      67,    59,    62,    35,    31,    30,    31,     4,    49,    36,
       5,     6,     5,     6,    66,     7,     7,    48,     5,     6,
      72,    36,    69,     5,     6,    87,    77,    48,    86,    36,
      94,    34,    98,    40,    73,    74,    75,    76,    78,    59,
      59,    59,    59,    84,    93,    95,    42,   100,    27,    96,
      40,    43,    31,     4,    89,    90,    91,    92,     5,     6,
       5,     6,     7,    42,    85,    31,     4,    79,     4,    68,
      50,     5,     6,     5,     6,     7,     0,     7,     4,    81,
       4,   101,     0,     5,     6,     5,     6,     7,     4,     7,
       5,     6,    60,     5,     6,     0,    26,     7,     5,     6,
       0,    18,    19,    20,    21,    82,    83
};

static const yysigned_char yycheck[] =
{
       4,    21,    19,     7,    21,    20,    18,    30,    20,    18,
      14,    20,     0,    13,     9,    19,    14,    21,    12,    23,
      43,     3,     4,    15,    28,    25,     8,    11,    10,    11,
      13,    35,    36,     3,     4,     3,     4,     5,    14,    21,
      10,    11,    10,    11,    48,    14,    14,    14,    10,    11,
       6,    21,     7,    10,    11,    70,    60,    14,    70,    21,
      80,    70,    85,    80,    17,    18,    19,    20,    12,    73,
      74,    75,    76,     5,    78,    15,    80,    97,    14,    83,
      97,     3,     4,     5,    73,    74,    75,    76,    10,    11,
      10,    11,    14,    97,     3,     4,     5,    63,     5,     6,
      24,    10,    11,    10,    11,    14,    -1,    14,     5,     6,
       5,     6,    -1,    10,    11,    10,    11,    14,     5,    14,
      10,    11,    12,    10,    11,    -1,     8,    14,    10,    11,
      -1,    12,    13,    12,    13,    15,    16
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    23,    24,     0,     5,    10,    11,    14,    25,    36,
      37,    38,    50,    51,    27,    28,    51,    51,    12,    13,
      12,    13,     9,    14,    38,    39,     8,    28,    12,    15,
       3,     4,    25,    30,    34,     3,    21,    34,    41,    42,
      45,    47,    51,     3,    25,    30,    41,    11,    14,    14,
      39,    38,    26,    51,    24,    31,    43,    44,    45,    51,
      12,    46,    51,    48,    24,    40,    51,    13,     6,     7,
      32,    35,     6,    17,    18,    19,    20,    51,    12,    35,
      49,     6,    15,    16,     5,     3,    25,    30,    33,    44,
      44,    44,    44,    51,    41,    15,    51,    29,    24,     8,
      41,     6
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)

# else

# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)

#else

#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
#line 79 "ipv2_parse.yy"
    { ip_pop_keyword(); ;}
    break;

  case 6:
#line 81 "ipv2_parse.yy"
    { ip_pop_keyword(); ;}
    break;

  case 7:
#line 83 "ipv2_parse.yy"
    { ip_pop_keyword(); ;}
    break;

  case 8:
#line 85 "ipv2_parse.yy"
    { ip_pop_keyword(); ;}
    break;

  case 9:
#line 87 "ipv2_parse.yy"
    { ip_begin_table(yyvsp[-1].sl); ;}
    break;

  case 10:
#line 90 "ipv2_parse.yy"
    { ip_done_table(); ;}
    break;

  case 15:
#line 109 "ipv2_parse.yy"
    { yyval.sl = ip_add_string_list(yyvsp[-1].sl,yyvsp[0].str); ;}
    break;

  case 16:
#line 111 "ipv2_parse.yy"
    { yyval.sl = ip_string_to_string_list(yyvsp[0].str); ;}
    break;

  case 17:
#line 115 "ipv2_parse.yy"
    { yyval.str = ip_append_keystrings(yyvsp[-2].str,yyvsp[0].str); ;}
    break;

  case 18:
#line 116 "ipv2_parse.yy"
    { yyval.str = yyvsp[0].str; ;}
    break;

  case 22:
#line 126 "ipv2_parse.yy"
    { ip_incr_karray(); ;}
    break;

  case 24:
#line 127 "ipv2_parse.yy"
    { ip_init_karray(); ;}
    break;

  case 28:
#line 135 "ipv2_parse.yy"
    { ip_start_karray(); ;}
    break;

  case 29:
#line 138 "ipv2_parse.yy"
    { ip_pop_karray(); ;}
    break;

  case 30:
#line 148 "ipv2_parse.yy"
    { ip_push_keyword(yyvsp[0].str); ;}
    break;

  case 31:
#line 149 "ipv2_parse.yy"
    { ip_push_keyclass(yyvsp[-1].str,yyvsp[0].str,0); ;}
    break;

  case 32:
#line 150 "ipv2_parse.yy"
    { ip_push_keyclass(yyvsp[-1].str,0,yyvsp[0].sl); ;}
    break;

  case 33:
#line 151 "ipv2_parse.yy"
    { ip_push_keyclass(yyvsp[-2].str,yyvsp[0].str,yyvsp[-1].sl); ;}
    break;

  case 34:
#line 152 "ipv2_parse.yy"
    { ip_push_keyclass(yyvsp[-2].str,yyvsp[-1].str,yyvsp[0].sl); ;}
    break;

  case 35:
#line 155 "ipv2_parse.yy"
    { ip_push_keyclass(0,yyvsp[0].str,0); ;}
    break;

  case 36:
#line 158 "ipv2_parse.yy"
    { yyval.str = yyvsp[-1].str; ;}
    break;

  case 37:
#line 161 "ipv2_parse.yy"
    { yyval.sl = yyvsp[-2].sl; ;}
    break;

  case 38:
#line 164 "ipv2_parse.yy"
    { yyval.sl = ip_add_string_list(yyvsp[-2].sl,yyvsp[0].str); ;}
    break;

  case 39:
#line 165 "ipv2_parse.yy"
    { yyval.sl = ip_string_to_string_list(yyvsp[0].str); ;}
    break;

  case 41:
#line 170 "ipv2_parse.yy"
    { ip_assign_value(yyvsp[0].str); ;}
    break;

  case 42:
#line 171 "ipv2_parse.yy"
    { ip_assign_variable(yyvsp[0].str); ;}
    break;

  case 43:
#line 172 "ipv2_parse.yy"
    { ip_assign_value(yyvsp[0].str); ;}
    break;

  case 44:
#line 176 "ipv2_parse.yy"
    { yyval.str = ip_double_to_string(yyvsp[-1].dbl); ;}
    break;

  case 45:
#line 179 "ipv2_parse.yy"
    { yyval.dbl = yyvsp[-2].dbl * yyvsp[0].dbl; ;}
    break;

  case 46:
#line 180 "ipv2_parse.yy"
    { yyval.dbl = yyvsp[-2].dbl - yyvsp[0].dbl; ;}
    break;

  case 47:
#line 181 "ipv2_parse.yy"
    { yyval.dbl = yyvsp[-2].dbl + yyvsp[0].dbl; ;}
    break;

  case 48:
#line 182 "ipv2_parse.yy"
    { yyval.dbl = yyvsp[-2].dbl / yyvsp[0].dbl; ;}
    break;

  case 49:
#line 186 "ipv2_parse.yy"
    { yyval.dbl = ip_get_variable_double(yyvsp[0].str); ;}
    break;

  case 50:
#line 187 "ipv2_parse.yy"
    { yyval.dbl = atof(yyvsp[0].str); free(yyvsp[0].str); ;}
    break;

  case 51:
#line 190 "ipv2_parse.yy"
    { yyval.str = yyvsp[0].str; ;}
    break;

  case 52:
#line 194 "ipv2_parse.yy"
    { yyval.str = ip_append_keystrings(yyvsp[-2].str,yyvsp[0].str); ;}
    break;

  case 53:
#line 195 "ipv2_parse.yy"
    { yyval.str = ip_append_keystrings(NULL,yyvsp[0].str); ;}
    break;

  case 54:
#line 196 "ipv2_parse.yy"
    { yyval.str = yyvsp[0].str; ;}
    break;

  case 56:
#line 202 "ipv2_parse.yy"
    { ip_incr_karray(); ;}
    break;

  case 58:
#line 204 "ipv2_parse.yy"
    { ip_init_karray(); ;}
    break;

  case 59:
#line 208 "ipv2_parse.yy"
    { yyval.str = (char*) malloc(strlen(yyvsp[-2].str)+strlen(yyvsp[0].str)+1);
                                  strcpy(yyval.str, yyvsp[-2].str);
                                  strcat(yyval.str, yyvsp[0].str);
                                  free(yyvsp[-2].str);
                                  free(yyvsp[0].str);
                                ;}
    break;

  case 60:
#line 215 "ipv2_parse.yy"
    { yyval.str = yyvsp[0].str; ;}
    break;

  case 61:
#line 218 "ipv2_parse.yy"
    { yyval.str = yyvsp[0].str; ;}
    break;

  case 62:
#line 219 "ipv2_parse.yy"
    { yyval.str = yyvsp[0].str; ;}
    break;


    }

/* Line 999 of yacc.c.  */
#line 1340 "ipv2_parse.tmp.cc"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 222 "ipv2_parse.yy"


