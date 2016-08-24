
#define    NODETYPE_LEAF   1
#define    NODETYPE_ID     2
#define     NODETYPE_PLUS  3
#define     NODETYPE_MINUS 4
#define     NODETYPE_MUL   5
#define   NODETYPE_DOT 60
#define   NODETYPE_DOTS 61
#define     NODETYPE_DIV   6
#define    NODETYPE_RET  30
#define     NODETYPE_GT    7
#define     NODETYPE_LT    8
#define     NODETYPE_GE    9
#define     NODETYPE_LE    10
#define   NODETYPE_INR   50
#define   NODETYPE_ALLOC 55
#define  NODETYPE_FREE 56
#define  NODETYPE_INITIAL 57
#define  NODETYPE_DCR 51
#define     NODETYPE_EQ    11
#define     NODETYPE_NE    12
#define     NODETYPE_IF    13
#define     NODETYPE_WHILE 14
#define     NODETYPE_READ  15
#define     NODETYPE_WRITE 16
#define     NODETYPE_SLIST 17
#define     NODETYPE_ASGN  18
#define    NODETYPE_AND 40
#define    NODETYPE_OR 41
#define    NODETYPE_MOD 42
#define     NODETYPE_IFELSE 19
#define    TTRUE 23
#define    TFALSE 0
#define    null  -1
#define    NODETYPE_BODY 33
#define    NODETYPE_EXPLIST 25
#define    NODETYPE_MAIN 26
#define    NODETYPE_FUNC 27



struct ASTNode {
    struct Typetable *type;
	int node;
	char *name;
	int val;
	struct ASTNode  *arglist;
	struct ASTNode *ptr1,*ptr2,*ptr3;
	struct Gsymbol *Gentry;
	struct Lsymbol *Lentry;
};


struct ASTNode * TreeCreate(char * ,int nodetype,int val,char *name,struct ASTNode *arglist,struct ASTNode *ptr1,struct ASTNode *ptr2,struct ASTNode *ptr3);

