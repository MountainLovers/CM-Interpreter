/* 域的关系链表 */
typedef struct domainNode
{
	struct domainNode * pre;
	int depth;
	int depthnum;
} DomainNode;