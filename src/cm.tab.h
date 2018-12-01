/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_CM_TAB_H_INCLUDED
# define YY_YY_CM_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUM = 258,
    ID = 259,
    PLUS = 260,
    SUB = 261,
    LT = 262,
    GT = 263,
    LET = 264,
    GET = 265,
    ET = 266,
    NET = 267,
    AND = 268,
    OR = 269,
    MUL = 270,
    DIV = 271,
    INT = 272,
    VOID = 273,
    ASSIGN = 274,
    UMINUS = 275,
    IF = 276,
    ELSE = 277,
    WHILE = 278,
    MAIN = 279,
    RETURN = 280,
    LPAREN = 281,
    RPAREN = 282,
    SEMI = 283,
    LBRACE = 284,
    RBRACE = 285,
    LBRACKET = 286,
    RBRACKET = 287,
    COMMA = 288,
    NEWLINE = 289,
    ERROR = 290
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 27 "cm.y" /* yacc.c:1909  */

	struct treeNode * node;
	int intval;
	double realval;
	char * idName;
	int operatorType;
	int dataType;

#line 99 "cm.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_CM_TAB_H_INCLUDED  */
