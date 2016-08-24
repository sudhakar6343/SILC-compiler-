


	void GInstall(char *name,struct Typetable  *type,int size,struct ArgStruct *arglist) {
		struct Gsymbol *check=GLookup(name);
		if(check) {
			printf("Error: variable %s  redeclared \n",name);
			exit(1);
		}
		struct Gsymbol *temp=(struct Gsymbol *)malloc(sizeof(struct Gsymbol));
			temp->name=malloc(16);
		strcpy(temp->name,name);
		if(size==0) {
			temp->size=0;
			temp->binding=fbinding++;
		}
		else if(strcmp(type->name,"integer")==0 || strcmp(type->name,"boolean")==0) {
	 		
	 		temp->size=size;
	 		temp->binding=gbinding;
	 		gbinding=gbinding+size;
		}
		else {
			temp->size=16*size;
			temp->binding=gbinding;
	 		gbinding=gbinding+size;
		}

		temp->arglist=arglist;
		temp->type=type;
		temp->next=NULL;
		if(GSymbolHead) {
			temp->next=GSymbolHead;
		
		}
			GSymbolHead=temp;
		
	}


struct Gsymbol* GLookup(char *name) {
	struct Gsymbol *temp=GSymbolHead;
	while(temp) {
		if(strcmp(temp->name,name)==0) {
	
			return temp;
		}
		temp=temp->next;
	}
	return temp;
}



struct Lsymbol * LInstall(char *name,struct Typetable *type,int ref) {
	    struct Lsymbol *temp=(struct Lsymbol *)malloc(sizeof(struct Lsymbol));
	    if(LLookup(name)) {
	    	printf("Redefined Variable %s \n ",name);
	    	exit(1);
	    };
        temp->name=(char *)malloc(16);
	   	strcpy(temp->name,name);
	   	temp->ref=ref;
	   
		if(ref)
		   		temp->binding=argbind--;
		else
				temp->binding=localbind++;
		temp->type=type;
		if(strcmp(type->name,"integer")==0 || strcmp(type->name,"boolean")==0)
			temp->size=1;
		else
			temp->size=16;
		
		temp->next=NULL;

		if(LSymbolHead) {
			temp->next=LSymbolHead;
	    }
	    LSymbolHead=temp;
}





struct Lsymbol* LLookup(char *name) {
	struct Lsymbol *temp=LSymbolHead;
	while(temp) {
		if(strcmp(temp->name,name)==0) {			
			return temp;
		}
		temp=temp->next;
	}
	return temp;
}


struct ArgStruct * Arginstall (char *name,struct Typetable *type,int passtype) {
        struct ArgStruct *temp=(struct ArgStruct *)malloc(sizeof(struct ArgStruct));
        temp->name=(char *)malloc(16);
       
        strcpy(temp->name,name);
        temp->type=type;
        temp->passType=passtype;
        temp->next=NULL;
       
        		
        
        	if(passtype==2) 
        	
        		LInstall(name,type,2);
        	
        	else 

        		LInstall(name,type,1);
        	
        
       
        return temp;
}


void Reinitialise(){   
	struct Lsymbol *temp=LSymbolHead;
	if(LSymbolHead) {
		temp=LSymbolHead->next;
	}

	while(LSymbolHead) {
	
		free(LSymbolHead->name);
		free(LSymbolHead);
		LSymbolHead=temp;
        if(LSymbolHead)
        	temp=temp->next;
	}

    LSymbolHead=NULL;
	localbind = 1;
	argbind = -3;
}

