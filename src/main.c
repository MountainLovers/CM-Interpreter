#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "analyze.h"
#include "domain.h"

/* 行号、源文件、输出文件 */
int lineno = 0;
FILE * source;
FILE * listing;

int Parse = TRUE;
int Analyze = TRUE;
int Execute = FALSE;

int printSyntaxTree = TRUE;

int Error = FALSE;

int main( int argc, char * argv[] )
{
	TreeNode * syntaxTree;
	char pgm[120]; /* 源文件名 */
	if (argc < 2)
	{	fprintf(stderr,"usage: %s <filename>\n",argv[0]);
		exit(1);
	}
	if (argc > 2)
	{
		if (strcmp(argv[2],"-s"))
			printSyntaxTree = TRUE;
		else
		{
			fprintf(stderr,"Unrecognized parameter.\n");
		}
	}
	strcpy(pgm,argv[1]) ;
	if (strchr (pgm, '.') == NULL)
		strcat(pgm,".cm");
	source = fopen(pgm,"r");
	if (source==NULL)
	{	fprintf(stderr,"File %s not found\n",pgm);
		exit(1);
	}
	listing = stdout;
	fprintf(listing,"\nCM Interpretation: %s\n",pgm);

	if (Parse)
	{	
		syntaxTree = parse();
		if (printSyntaxTree)
		{	fprintf(listing,"Syntax tree:\n");
			printTree(syntaxTree);
			fprintf(listing, "Print SyntaxTree Succeeded!\n");
		}
		if (! Error)
		{	
			if (Analyze)
			{
				buildSymTab(syntaxTree);
				if(! Error)
				{
					checkNode(syntaxTree);
					if(! Error)
						fprintf(listing,"Type check completed.\n");
					else
						fprintf(listing,"\nTypes of variables have got some errors.。\n");
				}
				else
					fprintf(listing,"\nResult:Some errors occurred in symbol table construction.\n");
			}
			
			/*
			if(!Error && Execute)
			{	fprintf(listing,"Executing....\n");
				execTree(syntaxTree);
				if(! Error)
					fprintf(listing,"\n\nResult:Execution Succeeded.\n");
				else
					fprintf(listing,"\n\nResult:Some errors occurred in Execution.\n");
			}*/
		}
	}
	fclose(source);
	return 0;
}
