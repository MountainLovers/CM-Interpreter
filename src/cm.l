/***********************************/
/* File: cm.l    version 1.0      */
/* Flex version for CM             */
/* CM Interpreter Construction     */
/* Author: Zhang Summary		   */
/***********************************/
%option noyywrap
%{
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "cm.tab.h"
%}

digit		0|1|2|3|4|5|6|7|8|9
letter		a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z
ID 			{letter}{letter}*
NUM 		{digit}{digit}*
whitespace	[ \t]*

%%

"if"		{ return IF;}
"else"		{ return ELSE;}
"while"		{ return WHILE;}
"int"		{ yylval.dataType = Int; return INT;} 
"void"		{ yylval.dataType = Void; return VOID;}
"return"	{ return RETURN;}

{NUM}		{ yylval.intval = atoi(yytext); return NUM;}
{ID}			{ yylval.idName = copyString(yytext); return ID;}
{whitespace}	{/*Do nothing.*/}

"+"		{ return PLUS;}
"-"		{ return SUB;}
"*"		{ return MUL;}
"/"		{ return DIV;}
"<"		{ return LT;}
">"		{ return GT;}
"<="	{ return LET;}
">="	{ return GET;}
"=="	{ return ET;}
"!="	{ return NET;}
"&&"	{ return AND;}
"||"	{ return OR;}

"="		{ return ASSIGN;}
"("		{ return LPAREN;}
")"		{ return RPAREN;}
";"		{ return SEMI;}
"{"		{ return LBRACE;}
"}"		{ return RBRACE;}
"["		{ return LBRACKET;}
"]"		{ return RBRACKET;}
","		{ return COMMA;}

"//"	{ char c = input();
		  while(c != '\n')
		  {	if (c == EOF) break;
		  	c = input();
		  }
		  lineno++;
		}
"/*"	{	char c;
			int flag = 1;
			do
			{	c = input();
				entry1:
				if (c == EOF) break;
				if (c == '\n') lineno++;
				if (c == '*')
				{	c = input();
					if (c == '/')
						flag = 0;
					else
						goto entry1;
				}
			} while (flag);
		}
\n		{lineno++;}
.		{yyerror("Mystery character %s\n", yytext);return ERROR;}

%%
/* 用于语法分析时初始化词法分析接口 */
void iniLexer(void)
{
	static int firstTime = TRUE;
	lineno = 0;
	if (firstTime)
	{	firstTime = FALSE;
		lineno++;
		yyin = source;
		yyout = listing;
	}
}
/* 词法分析器专用 
TokenType getToken(void)
{	static int firstTime = TRUE;
	TokenType currentToken;
	if (firstTime)
	{	firstTime = FALSE;
		lineno++;
		yyin = source;
		yyout = listing;
	}
	currentToken = yylex();
	strncpy(tokenString,yytext,MAXTOKENLEN);
	if (TraceScan) {
		fprintf(listing,"\t%d: ",lineno);
		printToken(currentToken);
	}
	return currentToken;
}*/
