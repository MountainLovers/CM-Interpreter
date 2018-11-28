/***********************************************************/
/* File: analyze.h                                         */
/* Analyze function declarations for the CMM interpreter   */
/* CMM Interpreter Construction                            */
/***********************************************************/

#ifndef _ANALYZE_H_
#define _ANALYZE_H_

/*	buildSymtab()先序遍历语法树，构造符号表。
 */
void buildSymTab(TreeNode *);

/*	checkNode()为类型检查。
	返回值为type.
	*/
int checkNode(TreeNode *);


#endif
