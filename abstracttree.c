 
struct ASTNode * TreeCreate(char *type,int nodetype,int val,char *name,struct ASTNode *arglist,struct ASTNode *ptr1,struct ASTNode *ptr2,struct ASTNode *ptr3) {
	struct Gsymbol *g;
    struct Typetable *t=getType(type);
	switch(nodetype) {
	case NODETYPE_PLUS :
	case NODETYPE_MINUS : 
	case NODETYPE_MUL : 
	case NODETYPE_MOD :
	case NODETYPE_DIV : 
	case NODETYPE_GE  : 
	case NODETYPE_LE : 
	case NODETYPE_GT : 
	case NODETYPE_LT  : 
		if(strcmp(ptr1->type->name,"integer")!=0 || strcmp(ptr2->type->name,"integer")!=0){
               printf("TreeCreate :Operands of arithmatic operation are not integers\n");
               exit(1);
        }
        break;

    case NODETYPE_ASGN :
      	if(ptr1->type!=ptr2->type  && strcmp(ptr2->type->name,"NULL")!=0) {
      		printf("Error: Assignment operands are not of same type %d %d\n",ptr1->type,ptr2->type);
      		exit(1);
      	}
        break;

    case NODETYPE_EQ : 
    case NODETYPE_NE  :
         
		if(ptr1->type!=ptr2->type && strcmp(ptr2->type->name,"NULL")!=0) {
               yyerror("TreeCreate : Operands  are not of same type");
               exit(1);
        }
        break;

     case NODETYPE_OR:
     case NODETYPE_AND   : if(!(strcmp(ptr1->type->name,"boolean") == 0 && strcmp(ptr1->type->name,"boolean") == 0)){
                    yyerror("mismatching operands for AND logical operation");exit(0);}
                      break;
	

      case NODETYPE_IF :
      case NODETYPE_IFELSE : 
      case NODETYPE_WHILE:
       		if(strcmp(ptr1->type->name,"boolean")!=0) {
       			yyerror("TreeCreate : Condition is not of type boolean\n");
       		}
       		break;
      case NODETYPE_INR :
      		if(strcmp(ptr1->type->name,"integer")!=0){
      			yyerror("TreeCreate: Increment operand is not of type int \n");
      			exit(1);
      		}
            break;
       case NODETYPE_DCR :
      		if(strcmp(ptr1->type->name,"integer")!=0){
      			yyerror("TreeCreate: Increment operand is not of type int \n");
      			exit(1);
      		}
            break;
      case NODETYPE_FUNC :
      		g=GLookup(name);
      		if(g==NULL) {
      			yyerror("TreeCreate:Undeclared Function %s\n",g);
      			exit(1);
      		}
      		struct ArgStruct *list=g->arglist;
      		struct ASTNode *t=arglist;

           if(t) {
      		while(t && list) {
               	if(list->passType==2) {
               		if(t->ptr2->node!=NODETYPE_ID && t->ptr2->node!=NODETYPE_DOT) {
               			printf("non-identifier passed as an arguement with passType as Pass By reference for the function %s",name);
               			exit(1);
               		}
               	}
               	else {
                  
               		if(t->ptr2->type!=list->type) {
               			printf("unmatching types passed for the function %s\n",name);
               			exit(1);
               		}
               	}
                list=list->next;
      			    t=t->ptr1;
      		}
      	}

      	if(t || list) {
      		printf("No of arguments to the function %s is not matching\n",name);
      		exit(1);
      	}

     }

     struct ASTNode *temp=(struct ASTNode *)malloc(sizeof(struct ASTNode ));
     temp->type=t;
     temp->node=nodetype;
  
    if(name) {
     temp->name=(char *)malloc(16);
     strcpy(temp->name,name);
     }
     temp->arglist=arglist;
     temp->val=val;
     temp->ptr1=ptr1;
     temp->ptr2=ptr2;
     temp->ptr3=ptr3;   
     return temp;
}


void validate(char *name,struct Typetable *type,struct ArgStruct *Arglist,struct ASTNode *body) {
	struct Gsymbol *temp=GLookup(name);
	if(temp==NULL) {
		printf("Undeclared function \n");
	}

    printf("%s %s",temp->name,temp->type->name);
	if(temp->type!=type) {
        printf("Error : Conflicting types of the function %s : %s %s type mismatch\n",name,temp->type->name,type->name);
        exit(1);
	}

	if(type!=body->ptr2->ptr1->type){
		printf("Error : Function type not matching with the function return type  %s \n",name);
		exit(1);
	}
  
    //validating the argument list 
	struct ArgStruct * Argtemp = temp->arglist;
	struct ArgStruct *ArgList = Arglist;
    while(Argtemp != NULL && ArgList != NULL){
     
        if(strcmp(Argtemp->name,ArgList->name) != 0){
            printf("Error : Conflicts in function %s : mismatch in arguement name %s", name, Argtemp->name);
            exit(1);
        }
       if(Argtemp->type != ArgList->type){
            printf("Error : Conflicts in function %s : %d %d mismatch in arguement type %s",name,Argtemp->type,ArgList->type,Argtemp->name);
            exit(1);
        }
        if(Argtemp->passType != ArgList->passType){
            printf("Error : Conflicts in function %s : mismatch in arguement passType %s", name,Argtemp->name);
            exit(1);
        }
        Argtemp = Argtemp->next;
        ArgList = ArgList->next; 
    }

	if(Argtemp || ArgList) {
		printf("Invalid number of arguments\n");
		exit(1);
	}
	
}


void setid(struct ASTNode * id) {
	   struct Gsymbol *temp1=GLookup(id->name);

      	    struct Lsymbol *temp2=LLookup(id->name);
      	    if(temp1==NULL && temp2==NULL) {
      	    	printf("Referenced to an undeclared variable %s\n",id->name);
      	    	exit(1);
      	    }
      	    if(temp2) {

      	    	 if(strcmp(temp2->type->name,"integer")!=0 &&  strcmp(temp2->type->name,"boolean")!=0) {
                   		if(temp2->size!=16) {
                   			printf("%s is a variable ,it is not an array \n",id->name);
   			 			    exit(1);
                   		}
                }

                else {
                   		if(temp2->size>1) {
                   			printf("%s is a variable  ,it is not an array \n",id->name);
   			 			          exit(1);
                   		}
                }

      	    	
   			      id->Lentry=temp2;
              id->type=temp2->type;
   			      
      	    }

      	    else {
                 
      	    	if(strcmp(temp1->type->name,"integer")!=0 &&  strcmp(temp1->type->name,"boolean")!=0) {
                   		if(temp1->size!=16) {
                   			printf("%s is not a variablea ,it is  an array \n",id->name);
   			 			          exit(1);
                   		}
                   	}

                   	else {
                   		if(temp1->size>1) {
                   			printf("%s is not a variable  ,it is  an array \n",id->name);
   			 			    exit(1);
                   		}
                   	}

   			    id->Gentry=temp1;
   			    id->type=temp1->type;
               
      	    }
}

void setida(struct ASTNode * ida) {
	        struct Gsymbol *temp1=GLookup(ida->name);
      	    
      	    if(temp1==NULL) {
      	    	printf("Referenced to an undeclared variable %s\n",ida->name);
              exit(1);
      	    }
      	    else {

      	    	if(strcmp(temp1->type->name,"integer")!=0 &&  strcmp(temp1->type->name,"boolean")!=0) {
                   		if(temp1->size==16) {
                   			printf("%s is a variable ,it is not an array \n",ida->name);
   			 			    exit(1);
                   		}
                   	}

                   	else {
                   		if(temp1->size==1) {
                   			printf("%s is a variable ,it is not an array \n",ida->name);
   			 			          exit(1);
                   		}
                   	}

      	    }	
   			    ida->Gentry=temp1;
   			    ida->type=temp1->type;   			    
      	          	 
}


