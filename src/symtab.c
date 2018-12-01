#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"
#include "domain.h"
/* SIZE 为符号表大小，是一个比较合适的素数 */
#define SIZE 211

/* SHIFT 用于移位运算  */
#define SHIFT 4

/* 哈希函数*/
static int hash ( char * key )
{	int temp = 0;
	int i = 0;
	while (key[i] != '\0')
	{	temp = ((temp << SHIFT) + key[i]) % SIZE;
		++i;
	}
	return temp;
}

/*	符号表表项，每一个表项包含：
	变量名，类型，声明时的行号，下一个表项的地址
 */
typedef struct SymbolRec
{	
	char * name;
	ExpType type;
	//LineList lines;
	int varLineno;
	union {	int int_val;
			double real_val; } value;
	union {	int * intArray;
			double * realArray; } array;
	int arrayLength;
	struct SymbolRec * next;
} * Symbol;

/* the hash table */
static Symbol hashTable[SIZE];

int insertSymTab( char * name, int dep, int depnum, int varLineno, int type, int arrayLength )
{	
	char *fenge = "-";
	char depthstr[10], depthnumstr[10];
	sprintf(depthstr, "%d", dep);
	sprintf(depthnumstr, "%d", depnum);
	char namet[100];
	strcpy(namet, name);
	strcat(namet, depthstr);
	strcat(namet, fenge);
	strcat(namet, depthnumstr);
	int h = hash(namet);
	Symbol s =  hashTable[h];
	/* 遍历符号表，查找是否有同名项 */
	while ((NULL != s)&&(0 != strcmp(name,s->name)))
		s = s->next;
	if(NULL == s)		/* 该符号不在符号表中，将其加入符号表，返回正确 */
	{	s = (Symbol) malloc(sizeof(struct SymbolRec));
		s->name = name;
		s->varLineno = varLineno;
		s->type = type;
		// debug fprintf(listing, "insertSymtab:%s\n", namet);
		if (0 > arrayLength)
		{	fprintf(listing,
				"Error:line %d:The length of array should be more than zero.\n", varLineno);
			Error = TRUE;
		}
		s->arrayLength = arrayLength;
		int tempArray[arrayLength];
		s->array.intArray = tempArray;
		s->next = hashTable[h];
		hashTable[h] = s;
		return arrayLength;
	}
	else	/* 若该符号在符号表中，报错 */
	{	fprintf(listing,
			"Error:line %d:Variable %s with the same name has been declared at line %d.\n",
				varLineno, name, s->varLineno);
		// debug fprintf(listing, "%d-%d  %s\n", dep, depnum, namet);
		Error = TRUE;
	}
	return -1;	//返回值为－1表示插入过程中出错。
}

int lookupSymTab( char * name, DomainNode * domain, int int_val, double real_val, int varLineno)
{	
	int flag = 0;
	char *fenge = "-";
	char depthstr[10], depthnumstr[10];
	char namet[100];
	while (domain)
	{
		if (domain->depth == -1 && domain->depthnum == -1) 
		{
			domain = domain->pre; 
			continue;
		}
		// debug fprintf(listing, "find:%s:%d-%d\n", name, domain->depth, domain->depthnum);
		sprintf(depthstr, "%d", domain->depth);
		sprintf(depthnumstr, "%d", domain->depthnum);
		strcpy(namet, name);
		strcat(namet, depthstr);
		strcat(namet, fenge);
		strcat(namet, depthnumstr);
		int h = hash(namet);
		Symbol s =  hashTable[h];
		while ((s != NULL)&&(0 != strcmp(name,s->name)))
			s = s->next;
		if (s != NULL)
		{
			if ((int_val != -1) || (real_val != -1))
			{	
				if (real_val != -1)
					s->value.real_val = real_val;
				else
					s->value.int_val = int_val;
			}
			return s->arrayLength;
		}
		domain = domain->pre;
	}
	fprintf(listing,
		"Error:line %d:Variable %s has not been declared.\n",varLineno, name);

	Error = TRUE;
	return -1;
	
}

int checkType(char * name, DomainNode * domain, int type, int varLineno)
{	
	int flag = 0;
	char *fenge = "-";
	char depthstr[10], depthnumstr[10];
	char namet[100];
	while (domain)
	{
		if (domain->depth == -1 && domain->depthnum == -1) 
		{
			domain = domain->pre; 
			continue;
		}
		// debug fprintf(listing, "find:%s:%d-%d\n", name, domain->depth, domain->depthnum);s
		sprintf(depthstr, "%d", domain->depth);
		sprintf(depthnumstr, "%d", domain->depthnum);
		strcpy(namet, name);
		strcat(namet, depthstr);
		strcat(namet, fenge);
		strcat(namet, depthnumstr);
		int h = hash(namet);
		Symbol s =  hashTable[h];
		while ((NULL != s)&&(0 != strcmp(name,s->name)))
			s = s->next;
		if(NULL != s)
		{	
			if (type != 0)
			{	if (type != s->type)
				{	fprintf(listing,
						"Error:line %d:Type of variable %s is not correct.",s->varLineno, name);
					Error = TRUE;
					return 1;
				}
				else
					return 0;
			}
			else
				return s->type;
			flag = 1;
			break;
		}
		domain = domain->pre;
	}
	if (flag == 0)
		{
			fprintf(listing,
				"Error:line %d:Variable %s has not been declared.\n",varLineno, name);
			Error = TRUE;
			return -1;
		}
}

double getValue(char * name)
{	int h = hash(name);
	Symbol s =  hashTable[h];
	while (0 != strcmp(name,s->name))
		s = s->next;
	return (double)s->value.int_val;
}

void updateValue(char * name, int int_val, double real_val)
{	int h = hash(name);
	Symbol s =  hashTable[h];
	while (0 != strcmp(name,s->name))
		s = s->next;
	s->value.int_val = int_val;
	return;
}

double getArray(char * name, int index)
{	int h = hash(name);
	Symbol s =  hashTable[h];
	while (0 != strcmp(name,s->name))
		s = s->next;
	return (double)s->array.intArray[index];
}

void updateArray(char * name, int index, int int_val, double real_val)
{	int h = hash(name);
	Symbol s =  hashTable[h];
	while (0 != strcmp(name,s->name))
		s = s->next;
	s->array.intArray[index] = int_val;
	return;
}

/*	printSymTab()打印符号表，验证符号表是否正确构造。
 */
void printSymTab(FILE * listing)
{	int i = 0;
	fprintf(listing,"\t/-----------------------------------\\\n");
	fprintf(listing,"\t|  VarName    Type IsArray DeclLine |\n");
	fprintf(listing,"\t|------------ ---- ------- ---------|\n");
	Symbol s = hashTable[i];
	while(i++ != SIZE)
	{	if(s != NULL)
		{	fprintf(listing,"\t|  %-11s",s->name);
			switch(s->type)
			{	case Int:
					fprintf(listing, "Int ");
					break;
				default:
					break;
			}
			if(0 == s->arrayLength)
			{	fprintf(listing,"    0");
				fprintf(listing,"      %-6d |\n", s->varLineno);
			}
			else
			{	fprintf(listing,"    %-4d   ",s->arrayLength);
				fprintf(listing,"%-6d |\n", s->varLineno);
			}
		}
		s = hashTable[i];
	}
	fprintf(listing,"\t\\-----------------------------------/\n");
} /* printSymTab end */
