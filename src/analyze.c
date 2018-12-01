/***********************************************************/
/* File: analyze.h                                         */
/* Analyze function implementation for the CM interpreter  */
/* CM Interpreter Construction                             */
/***********************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "domain.h"

/* 建立域的关系链表 */
static void beforeBFS(TreeNode *tree)
{
	int n = 0;
	
	DomainNode *rootDomainNode = (DomainNode *)malloc(sizeof(DomainNode));
	rootDomainNode->pre = NULL;
	rootDomainNode->depth = -1;
	rootDomainNode->depthnum = -1;
	while (tree)
	{
		// debug fprintf(listing, "GOINBFS%d\n", ++n);
		// debug fprintf(listing, "tree:%s:%d\n", tree->attr.name,tree);
		BFS(tree, rootDomainNode);
		tree = tree->sibling;
	}
}

void BFS(TreeNode * tree, TreeNode *root)
{
	// debug fprintf(listing, "BFS:\n");
	TreeNode *queue[100000];
	queue[1] = tree;
	DomainNode *nowdomain = (DomainNode *)malloc(sizeof(DomainNode));
	tree->domain = nowdomain;
	nowdomain->pre = root;
	nowdomain->depth = tree->depth;
	nowdomain->depthnum = tree->depthnum;
	// debug fprintf(listing, "%d/%d -> %d/%d\n", nowdomain->pre->depth,nowdomain->pre->depthnum,nowdomain->depth,nowdomain->depthnum);
	int head = 0, tail = 1;
	while (head < tail)
	{
		tree = queue[++head];
		// debug fprintf(listing, "bfstree:%d\n", tree);
		for (int i = 0; i<3; i++)
		{
			if (tree->child[i] != NULL)
			{
				TreeNode * now = tree->child[i];
				while (now)
				{
					// debug fprintf(listing, "NOWHERE1\n");
					queue[++tail] = now;
					// debug fprintf(listing, "bfschildtree:%d\n", now);
					if (!(now->depth == tree->depth && now->depthnum == tree->depthnum))
					{
						// debug fprintf(listing, "NOWHERE2\n");
						if (now->domain == NULL)
						{
							// debug fprintf(listing, "NOWHERE3\n");
							DomainNode *nowdomain = (DomainNode *)malloc(sizeof(DomainNode));

							now->domain = nowdomain;
							nowdomain->pre = tree->domain;
							nowdomain->depth = now->depth;
							nowdomain->depthnum = now->depthnum;
							// debug fprintf(listing, "ICANHERE\n");
							// debug fprintf(listing, "%d/%d -> %d/%d\n", nowdomain->pre->depth,nowdomain->pre->depthnum,nowdomain->depth,nowdomain->depthnum);
							
						}
					}
					else
					{
						now->domain = tree->domain;
					}
					now = now->sibling;
				}
				
			}
		}
	}
	// debug fprintf(listing, "BFSEND:\n");
}

/*	traverse()传入一个语法树节点，再传入一个遍历树的函数的指针
	这样既可以先序遍历，也可以后序遍历。
 */
static void traverse( TreeNode * tree,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{	if (tree != NULL)
	{	
		preProc(tree);
		int i;
		for (i=0; i < MAXCHILDREN; i++)
			traverse(tree->child[i],preProc,postProc);
		postProc(tree);
		traverse(tree->sibling,preProc,postProc);
	}
}

/*	nullProc()为traverse提供空函数指针，以便只执行一种遍历
 */
static void nullProc(TreeNode * tree)
{	if (tree==NULL) return;
	else return;
}

/*	insertNode()将节点中的变量插入符号表，
	并检查变量是否曾被声明。
 */
static void insertNode(TreeNode * tree)
{	
	switch (tree->nodekind)
	{	case StmtK:
			switch (tree->kind.stmt)
			{	case DeclK:
				case ParmK:
					insertSymTab(tree->attr.name,tree->depth,tree->depthnum,tree->lineno,tree->type,tree->arrayLength);
					break;
				case AssignK:
					checkType (tree->child[0]->attr.name, tree->child[0]->domain,0, tree->child[0]->lineno);
					break;
					break;
				case CompoundK:
				case IfK:
				case WhileK:
				default:
					break;
			}
			break;
		case ExpK:
			switch (tree->kind.exp)
			{	case IdK:
					tree->type = checkType(tree->attr.name,tree->domain,0,tree->lineno);
					break;
				case OpK:
				case IntValueK:
				default:
					break;
			}
			break;
		default:
			break;
	}
}

/*	buildSymtab()构造符号表
	使用先序遍历法
 */
void buildSymTab(TreeNode * syntaxTree)
{	
	beforeBFS(syntaxTree);
	traverse(syntaxTree,insertNode,nullProc);
	fprintf(listing,"\nSymbol table:\n");
	printSymTab(listing);
}
/* 类型错误时的处理函数。 */
static void typeError(TreeNode * tree, char * message)
{	fprintf(listing,"Error at line %d:Type error: %s\n",tree->lineno,message);
	Error = TRUE;
}
/* 类型不匹配时警告，默认强制转换为整型。 */
static void typeWarn(TreeNode * tree, char * message)
{	fprintf(listing,"\nWarn:Type conflict at line %d: %s",tree->lineno,message);
}

/*	checkNode()检查每个节点的类型，返回值为每个节点的类型
 */
int checkNode(TreeNode * tree)
{	TreeNode * treeTemp = tree;
	if (NULL == treeTemp)
	{	
		return 0;
	}
	int temp1,temp2;
	switch (treeTemp->nodekind)
	{	
		case ExpK:
			switch (treeTemp->kind.exp)
			{	
				case OpK:
					temp1 = checkNode(treeTemp->child[0]);
					if(NULL == treeTemp->child[1])	//检查是否是负的表达式
					{
						return Int;
					}
					else
					{	temp2 = checkNode(treeTemp->child[1]);
						if ( temp1 != temp2 )
						{	typeWarn(treeTemp,"Op applied to numbers which aren't the same type.");
							return Int;
						}
						else
							return Int;
					}
				case IdK:
					/* 验证此变量在声明时是不是数组 */
					temp1 = lookupSymTab (treeTemp->attr.name, treeTemp->domain, -1, -1, treeTemp->lineno);
					if ( 0 == temp1 )
					{	/* 验证结果:不是数组，若引用时该变量有下标则报错 */
						if (NULL == treeTemp->child[0] )
							return checkType(treeTemp->attr.name, treeTemp->domain, 0, treeTemp->lineno);
						else	//是数组元素但是没有下标
							typeError(treeTemp->child[0], "This variable is not an array element.");
					}
					else	//	验证结果:是数组，若引用时该变量没有下标则报错
					{	if ( NULL == treeTemp->child[0] )
							typeError(treeTemp->child[0], "This variable should be an array element.");
						else
							return checkType(treeTemp->attr.name, treeTemp->domain, 0, treeTemp->lineno);
					}
					break;
				case IntValueK:
					return Int;
				default:
					break;
			}
			break;
		case StmtK:
			switch (treeTemp->kind.stmt)
			{	
				case AssignK:
					/* 查询符号表确认变量在声明时是不是数组 */
					temp1 = lookupSymTab (treeTemp->attr.name, treeTemp->domain, -1, -1, treeTemp->child[0]->lineno);
					if (0 == temp1 )	//声明时不是数组
					{	/* 若引用时也不是数组元素 */
						if (NULL == treeTemp->child[1])
						{	/* 最后确认赋值符左右的值类型一致，不一致则警告并返回，一致则直接返回 */
							if (checkType(treeTemp->attr.name, treeTemp->domain, 0, treeTemp->child[0]->lineno) !=
									checkNode(treeTemp->child[0]))
								typeWarn(treeTemp->child[0],
									"Variable and right value are not the same type.");
							break;
						}
						else	//若变量被当成了数组元素
						{	typeError(treeTemp->child[0],
								"This variable is not an array element.");
							break;
						}
					}
					else	//该变量是数组
					{	if (NULL == treeTemp->child[1])	//若没有下标
						{	typeError(treeTemp->child[0],
								"This variable should be an array element.");
							break;
						}
						else	//有下标
						{	if (Int != checkNode(treeTemp->child[0] ))	//若数组下标不为整数
							{	typeError(treeTemp->child[0], "The index must be an integer.");
								break;
							}
							else
							{	if (checkType(treeTemp->attr.name, treeTemp->domain, 0, treeTemp->child[0]->lineno) !=
										checkNode(treeTemp->child[1]))
									typeWarn(treeTemp->child[0],
									"Variable and right value are not the same type.");
								break;
							}
						}
					}
				case DeclK:
					break;
				case WhileK:
					checkNode(treeTemp->child[0]);
					checkNode(treeTemp->child[1]);
					break;
				case CompoundK:
					checkNode(treeTemp->child[0]);
					break;
				case IfK:
					checkNode(treeTemp->child[0]);
					checkNode(treeTemp->child[1]);
					checkNode(treeTemp->child[2]);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	treeTemp = treeTemp->sibling;
	checkNode(treeTemp);
	return 0;	//	开头的返回值0与此处的0表示检查完成，正常返回，如while,if,write,compound等。
}
