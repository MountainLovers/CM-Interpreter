/*************************************************************/
/* File: util.c                                              */
/* Utility function implementation for the CM interpreter    */
/* CMM Interpreter Construction                              */
/* Author: Zhang Summary								     */
/*************************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/*	词法分析器接口：
	printToken 输出每个Token和它的类型
	token代表Token的类型，tokenString代表当前token的具体内容
*/
void printToken( TokenType token )
{	switch (token)
	{	case IF:
		case ELSE:
		case WHILE:
		case INT:
		case VOID:
			fprintf(listing,
			"reserved word: %s\n", tokenString);
			break;
		case NUM:
			fprintf(listing,
			"Int value: %s\n", tokenString);
			break;
		case ID:
			fprintf(listing,
			"Identifier, name: %s\n", tokenString);
			break;
		case PLUS: fprintf(listing, "+\n"); break;
		case SUB: fprintf(listing, "-\n"); break;
		case MUL: fprintf(listing, "*\n"); break;
		case DIV: fprintf(listing, "/\n"); break;
		case UMINUS: fprintf(listing, "-\n"); break;
		case LT: fprintf(listing, "<\n"); break;
		case GT: fprintf(listing, ">\n"); break;
		case LET: fprintf(listing, "<=\n"); break;
		case GET: fprintf(listing, ">=\n"); break;
		case ET: fprintf(listing, "==\n"); break;
		case NET: fprintf(listing, "!=\n"); break;
		case AND: fprintf(listing, "&&\n"); break;
		case OR: fprintf(listing, "||\n"); break;
		case ASSIGN: fprintf(listing, "=\n"); break;
		case LPAREN: fprintf(listing, "(\n"); break;
		case RPAREN: fprintf(listing, ")\n"); break;
		case SEMI: fprintf(listing, ";\n"); break;
		case LBRACE: fprintf(listing, "{\n"); break;
		case RBRACE: fprintf(listing, "}\n"); break;
		case LBRACKET: fprintf(listing, "[\n"); break;
		case RBRACKET: fprintf(listing, "]\n"); break;
		case ENDFILE: fprintf(listing, "EOF\n"); break;
		case ERROR:
			fprintf(listing,
			"ERROR: %s\n", tokenString);
			break;
		default:/* should never happen */
			fprintf(listing, "Unknown token: %d\n", token);
		}
}	//	printToken end

/*	语法分析器接口：
*/

/* newStmtNode 创建一个新的语句节点 */
TreeNode * newStmtNode(StmtKind kind)
{	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	if (t==NULL)
		fprintf(listing,"Out of memory error at line %d\n",lineno);
	else
	{	/* 初始化一个没有子节点和兄弟节点的语句节点 */
		for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = StmtK;
		t->kind.stmt = kind;
		t->lineno = lineno;
		t->type = Int;
	}
	return t;
}

/* newExpNode 创建一个新的表达式节点 */
TreeNode * newExpNode(ExpKind kind)
{	TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	if (t==NULL)
		fprintf(listing,"Out of memory error at line %d\n",lineno);
	else
	{
		for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = ExpK;
		t->kind.exp = kind;
		t->lineno = lineno;
		t->type = Int;
	}
	return t;
}

/* copyString 分配一块内存以放置新的字符串，并返回其首地址 */
char * copyString(char * s)
{	int n;
	char * t;
	if (s==NULL) return NULL;
	n = strlen(s)+1;
	t = malloc(n);
	if (t==NULL)
		fprintf(listing,"Out of memory error at line %d\n",lineno);
	else strcpy(t,s);
	return t;
}

/* 缩进数 */
static int indentCount = 0;

/* 改变需要输出的缩进的数目——宏定义 */
#define INDENT indentCount+=2
#define UNINDENT indentCount-=2

/* printSpaces 通过输出空格实现缩进 */
static void printSpaces(void)
{	int i;
	for (i=0;i<indentCount;i++)
	fprintf(listing," ");
}

/* printTree 输出语法树 */
void printTree(TreeNode *tree)
{
	INDENT;
	while (tree != NULL)
	{
		printSpaces();
		if (tree->nodekind == StmtK)
		{
			switch(tree->kind.stmt)
			{
				case IfK:
					fprintf(listing, "IF\n");
					printTree(tree->child[0]);
					printSpaces();
					fprintf(listing, "THEN\n");
					printTree(tree->child[1]);
					if (tree->child[2] != NULL)
					{
						printSpaces();
						fprintf(listing, "ELSE\n");
						printTree(tree->child[2]);
					}
					break;
				case DeclK:
					fprintf(listing,"Declaration:\n");
					INDENT;
					printSpaces();
					fprintf(listing,"Type: ");
					if (tree->type == Int)
					{
						fprintf(listing,"int ");
						if(0 != tree->arrayLength)	//是数组
							fprintf(listing,"array of %d elements\n",tree->arrayLength);
						else fprintf(listing,"\n");		//不是数组
					}
					else
					{
						fprintf(listing,"Unknown Declaration Type\n");
					}
					printSpaces();
					fprintf(listing,"Id: %s\n",tree->attr.name);
					UNINDENT;
					break;
				case CompoundK:
					fprintf(listing, "CompStmt:\n");
					if (tree->child[0] != NULL)
					{
						INDENT;
						printSpaces();
						fprintf(listing, "Local_Declaration:\n");
						printTree(tree->child[0]);
						UNINDENT;
					}
					INDENT;
					printSpaces();
					fprintf(listing, "What_to_Do:\n");
					printTree(tree->child[1]);
					UNINDENT;
					break;
				case WhileK:
					fprintf(listing, "WHILE:\n");
					printTree(tree->child[0]);
					printSpaces();
					fprintf(listing, "DO:\n");
					printTree(tree->child[1]);
					break;
				case AssignK:
					fprintf(listing, "Assign: \n");
					INDENT;
					printSpaces();
					fprintf(listing, "To: \n");
					printTree(tree->child[0]);
					printSpaces();
					fprintf(listing, "Value: \n");
					printTree(tree->child[1]);
					UNINDENT;
					break;
				case FuncK:
					fprintf(listing, "Function:\n");
					INDENT;
					printSpaces();
					fprintf(listing, "Return_Value_Type: ");
					switch (tree->type)
					{
						case Int:
							fprintf(listing, "int\n");
							break;
						case Void:
							fprintf(listing, "void\n");
							break;
						default:
							fprintf(listing, "Unknown Type\n");
							fprintf(listing, "%d\n", tree->type);
					}
					printSpaces();
					fprintf(listing, "Function_Name: %s\n", tree->attr.name);
					if (tree->child[0] == NULL) 
					{	
						printSpaces();
						fprintf(listing, "Params:\n");
						INDENT;
						printSpaces();
						fprintf(listing, "Type: VOID\n");
						UNINDENT;
					}
					else
					{
						UNINDENT;
						printTree(tree->child[0]);
					}
					break;
				case ParmK:
					fprintf(listing, "Params:\n");
					INDENT;
					printSpaces();
					fprintf(listing,"Type: ");
					switch (tree->type)
					{
						case Int:
							fprintf(listing,"int ");
							if(-1 == tree->arrayLength)	//是指针
								fprintf(listing,"*\n");
							else fprintf(listing,"\n");		//不是指针
							break;
						case Void:
							fprintf(listing, "void \n");
							break;
						default:
							fprintf(listing,"Unknown Declaration Type\n");
							fprintf(listing, "%d\n", tree->type);
					}
					printSpaces();
					fprintf(listing,"Variable_Name: %s\n",tree->attr.name);
					UNINDENT;
					break;
				case ReturnK:
					fprintf(listing, "Function_Return: \n");
					if (tree->child[0] == NULL)
					{
						INDENT;
						printSpaces();
						fprintf(listing, "void\n");
						UNINDENT;
					}
					else
					{
						printTree(tree->child[0]);
					}
					break;
				default:
					fprintf(listing,"Unknown StmtNode kind\n");
					break;
			}
		}
		else if (tree->nodekind == ExpK)
		{
			switch (tree->kind.exp)
			{
				case OpK:
					fprintf(listing,"Op: ");
					switch(tree->attr.op)
					{
						case PLUS: fprintf(listing, "+\n"); break;
						case SUB: fprintf(listing, "-\n"); break;
						case MUL: fprintf(listing, "*\n"); break;
						case DIV: fprintf(listing, "/\n"); break;
						case UMINUS: fprintf(listing, "--\n"); break;
						case LT: fprintf(listing, "<\n"); break;
						case GT: fprintf(listing, ">\n"); break;
						case LET: fprintf(listing, "<=\n"); break;
						case GET: fprintf(listing, ">=\n"); break;
						case ET: fprintf(listing, "==\n"); break;
						case NET: fprintf(listing, "!=\n"); break;
						case AND: fprintf(listing, "&&\n"); break;
						case OR: fprintf(listing, "||\n"); break;
						default: fprintf(listing, "error!!! %d\n", tree->attr.op);
					}
					printSpaces();
					fprintf(listing, "Left: \n");
					printTree(tree->child[0]);
					printSpaces();
					fprintf(listing, "Right: \n");
					printTree(tree->child[1]);
					break;
				case IntValueK:
					fprintf(listing,"Int value: %d\n",tree->value.int_val);
					break;
		    	case IdK:
					if (tree->child[0] == NULL)
						fprintf(listing,"Variable_Name: %s\n",tree->attr.name);
					else
					{
						fprintf(listing,"Variable_Name: %s[]\n",tree->attr.name);
						INDENT;
						printSpaces();
						fprintf(listing, "Array Index:\n");
						printTree(tree->child[0]);
						UNINDENT;
					}
					break;
				case CallK:
					fprintf(listing, "Function_Call: \n");
					INDENT;
					printSpaces();
					fprintf(listing, "Called_Function_Name: %s\n", tree->attr.name);
					UNINDENT;
					printTree(tree->child[0]);
					break;
		    	default:
					fprintf(listing,"Unknown ExpNode kind\n");
					break;
			}

		}
		else
			fprintf(listing,"Unknown node kind\n");
		tree = tree->sibling;
	}
	UNINDENT;
}
