struct Typetable {
	char *name;
	struct Fieldlist *fields;
	struct Typetable *next;
};

struct Fieldlist {
	char *name;
	int no;
	struct Typetable *type;
	struct FieldList *next;
};

struct Typetable *typeHead;
struct Fieldlist *fieldHead;
int no=1;
struct Typetable * getType(char *name);
void createField(char *typename,char *name);
void createType(char *name);
