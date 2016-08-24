struct ArgStruct{
	struct Typetable *type;
	int passType;
    char *name; 
	struct ArgStruct *next;
};

struct Gsymbol {
	char *name;
	int binding;
	struct Typetable *type;
	int size;
	struct Lsymbol *lsymbol;
	struct ArgStruct *arglist;
	struct Gsymbol *next;
};


struct Lsymbol{
  char *name;               //name of the variable
  struct Typetable *type;  //pointer to the Typetable entry of variable type
  int binding;              //stores memory address allocated to the variable 
  int ref;
  int size;
  struct Lsymbol *next;     //points to the next Local Symbol Table entry
};

	int fbinding=4;
	int gbinding=1650;


struct Gsymbol *GSymbolHead;
struct Lsymbol *LSymbolHead;
void GInstall(char *name,struct Typetable *type,int size,struct ArgStruct *arglist) ;
struct Lsymbol * LInstall(char *name,struct Typetable *type,int ref);
struct Gsymbol* GLookup(char *name);
struct Lsymbol* LLookup(char *name);


