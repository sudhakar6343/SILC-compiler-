void createType(char *name) {
	struct Typetable *temp=getType(name);
	if(temp==NULL)
		temp=(struct Typetable *)malloc(sizeof(struct Typetable));
	else {
		printf("Error:User defined type %s \n is already declared\n",name);
		exit(1);
	}
    temp->name=(char *)malloc(16);
    strcpy(temp->name,name);
    temp->fields=NULL;
    temp->next=NULL;
    if(typeHead) 
    	temp->next=typeHead;
    typeHead=temp;
}


void createField(char *typename,char *name) {
	struct Fieldlist *temp=(struct Fieldlist * )malloc(sizeof(struct Fieldlist));
	struct Typetable *t=getType(typename);
	if(t==NULL) {
		printf("Error:Reference to an undeclared type %s\n",typename);
		exit(1);
	}
	temp->type=t;
	temp->no=no;

	no++;
	temp->name=(char *)malloc(16);
	strcpy(temp->name,name);
	temp->next=NULL;
	if(fieldHead) {
		temp->next=fieldHead;
	}
	fieldHead=temp;
}

struct Fieldlist * checkfield(struct Typetable *t,char *name) {
    struct Fieldlist *temp=t->fields;
    while(temp) {
    	if(strcmp(temp->name,name)==0) {
    	
    		return temp;
    	}
    	temp=temp->next;
    }
    printf("Error:There is no variable %s in type %s\n",name,t->name);
    exit(1);
}

struct Typetable * getType(char *name) {
	struct Typetable *t=typeHead;
	while(t) {
		if(strcmp(t->name,name)==0) {
			return t;
		}
		t=t->next;
	}
	return t;
}

