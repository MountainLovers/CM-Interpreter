/***********************************/
/* File: cm.y                      */
/* Bison grammar file about CM     */
/* CM Interpreter Construction     */
/* Author: Zhang Summary		   */
/***********************************/

/*预计有1个移进/归约冲突*/
%expect 1
%{

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TreeNode * savedTree; /* stores syntax tree for later return */

//最大递归深度10，也就是最大嵌套深度
#define MAXDEPTH 10

int Depth;
int DepthNum[MAXDEPTH];

%}

%union{
	struct treeNode * node;
	int intval;
	double realval;
	char * idName;
	int operatorType;
	int dataType;
}
%token <intval> NUM
%token <idName> ID
%token <operatorType> PLUS SUB LT GT LET GET ET NET AND OR MUL DIV
%token <dataType> INT VOID

/* 优先级声明 */
%right ASSIGN
%left PLUS SUB
%left MUL DIV
%nonassoc LT GT LET GET ET NET AND OR
%nonassoc UMINUS
/* 声明文法中用到的tokens */
%token IF ELSE WHILE MAIN RETURN
%token LPAREN RPAREN SEMI LBRACE RBRACE LBRACKET RBRACKET COMMA
%token ASSIGN
%token NEWLINE ERROR

%type <node> decl_list stat params params_list local_decl stat_list fun_decl arg_list 
%type <node> sele_stmt decl compound_stmt iter_stmt var_decl exp_stmt return_stmt call if_last_stmt
%type <node> exp factor param var simp_exp add_exp term args
%type <node> addop mulop relop
%type <dataType> type-specifier

%start program

%% /* CM文法 */

program		: decl_list { savedTree = $1;}
			;
decl_list	:	{ $$ = NULL; }
			| decl_list decl 
				{	
					TreeNode * t = $1;
					if (t != NULL)
					{	while (t->sibling != NULL){ t = t->sibling;}
						t->sibling = $2;
						$$ = $1;
					}
					else $$ = $2;
				}
			;
decl 		: var_decl { $$ = $1; }
			| fun_decl { $$ = $1; }
			| error { $$ = NULL; }
			;
var_decl	: type-specifier ID SEMI 
				{	
					$$ = newStmtNode(DeclK);
					$$->attr.name = $2;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "decl:%s:%d/%d\n", $$->attr.name, $$->depth, $$->depthnum);
					/* 数组长度为0代表非数组 */
					$$->arrayLength = 0;
					$$->type = $1;
					$$->lineno = lineno;
					// debug fprintf(listing, "decltree:%s:%d\n", $$->attr.name, $$);
				}
			| type-specifier ID LBRACKET NUM RBRACKET SEMI
				{	$$ = newStmtNode(DeclK);
					$$->attr.name = $2;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "decl:%s:%d/%d\n", $$->attr.name, $$->depth, $$->depthnum);
					$$->arrayLength = $4;
					$$->type = $1;
					if($$->type == Int) {
						int int_array_temp[$4];
						$$->array.intArray = int_array_temp;
					}
					$$->lineno = lineno;
				}
			;
type-specifier	
			: INT 	{ $$ = $1; }
			| VOID  { $$ = $1; }
			;
fun_decl	: type-specifier ID 
				{
					Depth = Depth+1; 
					DepthNum[Depth] = DepthNum[Depth]+1;
				}
			  LPAREN params RPAREN
				{
					$$ = newStmtNode(FuncK);
					$$->type = $1;
					$$->attr.name = $2;
					$$->child[0] = $5;
					$$->lineno = lineno;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "funtree:%s:%d\n", $$->attr.name, $$);
				}
			| compound_stmt 
				{ 
					$$ = $1; 
					Depth = Depth-1;
					// debug fprintf(listing, "comptree:%d\n", $$);
				}
			; 
params		: VOID { $$ = NULL; }
			| params_list { $$ = $1; }
			;
params_list	: params_list COMMA param
				{
					TreeNode *t = $1;
					if (t != NULL)
					{
						while (t->sibling != NULL) t = t->sibling;
						t->sibling = $3;
					}
					else $$ = $3;
				}
			| param { $$ = $1; }
			;
param 		: type-specifier ID
				{
					$$ = newStmtNode(ParmK);
					$$->attr.name = $2;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "decl:%s:%d/%d\n", $$->attr.name, $$->depth, $$->depthnum);
					$$->arrayLength = 0;
					$$->type = $1;
					$$->lineno = lineno;
				}
			| type-specifier ID LBRACKET RBRACKET
				{
					$$ = newStmtNode(ParmK);
					$$->attr.name = $2;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "decl:%s:%d/%d\n", $$->attr.name, $$->depth, $$->depthnum);
					$$->arrayLength = -1;
					$$->type = $1;
					$$->lineno = lineno;
				}
			;
compound_stmt
			: LBRACE local_decl stat_list RBRACE
				{
					$$ = newStmtNode(CompoundK);
					$$->child[0] = $2;
					$$->child[1] = $3;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					$$->lineno = lineno;
				}
			;
local_decl	: { $$ = NULL; }
			| local_decl var_decl
				{	
					TreeNode * t = $1;
					if (t != NULL)
					{	
						while (t->sibling != NULL){ t = t->sibling;}
						t->sibling = $2;
						$$ = $1;
					}
					else $$ = $2;
				}
			;
stat_list	: { $$ = NULL; }
			| stat_list stat
				{	
					TreeNode * t = $1;
					if (t != NULL)
					{	
						while (t->sibling != NULL){ t = t->sibling;}
						t->sibling = $2;
						$$ = $1;
					}
					else $$ = $2;
				}
			;
stat 		: exp_stmt {$$ = $1;}
			| compound_stmt {$$ = $1;}
			| sele_stmt {$$ = $1;}
			| iter_stmt {$$ = $1;}
			| return_stmt {$$ = $1;}
			| error {$$ = NULL;}
			;
exp_stmt	: exp SEMI {$$ = $1;}
			| SEMI {$$ = NULL;}
			;
sele_stmt   : IF
				{
					Depth++;
					DepthNum[Depth]++;
				} 
			  if_last_stmt
			  	{
			  		$$ = $3;
			  	}
			;
if_last_stmt: LPAREN exp RPAREN stat
				{	
					$$ = newStmtNode(IfK);
					$$->child[0] = $2;
					$$->child[1] = $4;
					$$->child[2] = NULL;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					$$->lineno = lineno;
					// debug fprintf(listing, "iftree:%d at %d\n", $$, $$->lineno);
					// debug fprintf(listing, "if:%d/%d at %d\n", $$->depth, $$->depthnum, $$->lineno);
					Depth--;
				}
			| LPAREN exp RPAREN stat ELSE stat
				{	
					$$ = newStmtNode(IfK);
					$$->child[0] = $2;
					$$->child[1] = $4;
					$$->child[2] = $6;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					$$->lineno = lineno;
					// debug fprintf(listing, "iftree:%d at %d\n", $$, $$->lineno);
					// debug fprintf(listing, "if:%d/%d at %d\n", $$->depth, $$->depthnum, $$->lineno);
					Depth--;
				}
			;
iter_stmt	: WHILE
				{
					Depth++;
					DepthNum[Depth]++;
				} 
			  LPAREN exp RPAREN stat
				{	$$ = newStmtNode(WhileK);
					$$->child[0] = $4;
					$$->child[1] = $6;
					$$->lineno = lineno;
					Depth--;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth]-1;
					// debug fprintf(listing, "whiletree:%d\n", $$);
				}
			;
return_stmt	: RETURN SEMI 
				{
					$$ = newStmtNode(ReturnK);
					// debug fprintf(listing, "returntree:%d at %d\n", $$, $$->lineno);
					$$->child[0] = NULL;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					$$->lineno = lineno;
					// debug fprintf(listing, "return:%d/%d at %d\n", $$->depth, $$->depthnum, $$->lineno);
				}
			| RETURN exp SEMI
				{
					$$ = newStmtNode(ReturnK);
					// debug fprintf(listing, "returntree:%d at %d\n", $$, $$->lineno);
					$$->child[0] = $2;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					$$->lineno = lineno;
					// debug fprintf(listing, "return:%d/%d at %d\n", $$->depth, $$->depthnum, $$->lineno);
				}
			;
exp 		: var ASSIGN exp
				{
					$$ = newStmtNode(AssignK);
					$$->child[0] = $1;
					$$->child[1] = $3;
					$$->lineno = lineno;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "Exp:%d/%d  %d\n", $$->depth, $$->depthnum, $$->lineno);
					// debug fprintf(listing, "Assign:%d\n", $$);
				}
			| simp_exp {$$ = $1;}
			;
var 		: ID
				{
					$$ = newExpNode(IdK);
					$$->attr.name = $1;
					//	child[0] is NULL means this is not array;
					$$->child[0] = NULL;
					$$->lineno = lineno;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "decl:%s:%d/%d\n", $$->attr.name, $$->depth, $$->depthnum);
					// debug fprintf(listing, "varuse:%s:%d\n", $$->attr.name, $$);
				}
			| ID LBRACKET exp RBRACKET
				{
					$$ = newExpNode(IdK);
					$$->attr.name = $1;
					//	child[0] not NULL means x in array[x]
					$$->child[0] = $3;
					$$->lineno = lineno;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "decl:%s:%d/%d\n", $$->attr.name, $$->depth, $$->depthnum);
				}
			;
simp_exp 	: add_exp relop add_exp
				{
					$$ = newExpNode(OpK);
					$$->child[0] = $1;
					$$->child[1] = $3;
					$$->attr.op = $2;
					$$->lineno = lineno;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "simp_exp:%d/%d  %d\n", $$->depth, $$->depthnum, $$->lineno);
				}
			| add_exp { $$ = $1; }
			;
relop 		: LET { $$ = LET; }
			| LT { $$ = LT; }
			| GT { $$ = GT; }
			| GET { $$ = GET; }
			| ET { $$ = ET; }
			| NET { $$ = NET; }
			;
add_exp 	: add_exp addop term
				{
					$$ = newExpNode(OpK);
					$$->child[0] = $1;
					$$->child[1] = $3;
					$$->attr.op = $2;
					$$->lineno = lineno;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "add_exp:%d/%d  %d\n", $$->depth, $$->depthnum, $$->lineno);
				}
			| term {$$ = $1;}
			;
addop		: PLUS { $$ = PLUS; }
			| SUB { $$ = SUB; }
			;
term		: term mulop factor
				{
					$$ = newExpNode(OpK);
					$$->child[0] = $1;
					$$->child[1] = $3;
					$$->attr.op = $2;
					$$->lineno = lineno;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "term:%d/%d  %d\n", $$->depth, $$->depthnum, $$->lineno);
				}
			| factor {$$ = $1;}
			;
mulop 		: MUL { $$ = MUL; }
			| DIV { $$ = DIV; }
			;
factor 		: LPAREN exp RPAREN {$$ = $2;}
			| var {$$ = $1;}
			| call {$$ = $1;}
			| NUM 
				{	
					$$ = newExpNode(IntValueK);
					$$->value.int_val = $1;
					$$->type = Int;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					$$->lineno = lineno;
				}
			;
call 		: ID LPAREN args RPAREN
				{
					$$ = newExpNode(CallK);
					// debug fprintf(listing, "calltree:%s:%d\n", $$->attr.name, $$);
					$$->attr.name = $1;
					$$->child[0] = $3;
					$$->depth = Depth;
					$$->depthnum = DepthNum[Depth];
					// debug fprintf(listing, "call:%s:%d/%d at %d\n",$$->attr.name, $$->depth, $$->depthnum, $$->lineno);
				}
			;
args 		: {$$ = NULL;}
			| arg_list {$$ = $1;}
			;
arg_list 	: arg_list COMMA exp
				{
					TreeNode *t = $1;
					if (t != NULL)
					{
						while (t->sibling != NULL) t = t->sibling;
						t->sibling = $3;
					}
					else $$ = $3;
				}
			| exp {$$ = $1;}
			;

%%

int yyerror(char * message)
{	// debug fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
	// debug fprintf(listing,"Current token: %s",tokenString);
	printToken(yychar);
	Error = TRUE;
	return 0;
}
/* 与主函数交互的语法分析函数 */
TreeNode * parse(void)
{	iniLexer();
	yyparse();
	return savedTree;
}
