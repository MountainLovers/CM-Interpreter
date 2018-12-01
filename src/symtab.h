struct domainNode;
typedef struct domainNode DomainNode;

/*	searchSymTab()用于查找和插入符号表，并协助类型检查。
	参数说明：
	name是当前的变量名，
	varLineno是当前行号，用于错误提示，
	type为变量声明时需要填入符号表的符号类型。
	arrayLength为数组长度，为－123代表非数组，其他的小于0的值报错。
	返回值：
	正确返回时为arrayLength，遇错为－1；
	*/
int insertSymTab( char * name, int dep, int depnum, int varLineno, int type, int arrayLength );

/*	查找符号表，若找不到则返回－1，否则返回变量的arrayLength，
	arrayLength为0表示非数组，arrayLength非0表示下标的值。
	*/
int lookupSymTab ( char * name, DomainNode * domain, int int_val, double real_val, int varLineno );

/*	参数type为0时，直接返回变量在符号表中的类型；
	type不为0时，检查变量名的类型与传入的type是否一致，一致返回0，不一致返回1。
	*/
int checkType(char * name, DomainNode * domain, int type, int varLineno);

/*	printSymTab()打印符号表，验证符号表是否正确构造。
	*/
void printSymTab(FILE * listing);

/*	以下为解释执行时所用的函数，分别为：
	得到变量的值，更新变量的值，
	得到数组元素的值，更新数组元素的值。
	*/
double getValue(char * name);

void updateValue(char * name, int int_val, double real_val);

double getArray(char * name, int index);

void updateArray(char * name, int index, int int_val, double real_val);

