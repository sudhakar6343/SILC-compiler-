%{
	#include<stdio.h>
	#include<stdlib.h>
    #include<string.h>
    int counter=0;
   

	int localbind=1;
	int argbind=-3;

  #include "typetable.h"
  #include "abstracttree.h"
	#include "symboltable.h"
	#include "typetable.c"
	#include "codegen.h"
  #include "abstracttree.c"
	#include "symboltable.c"
  #include "codegen.c"

   
    struct Typetable *tt,*decl_type,*decl_type2,*decl_type3,*decl_typeu;         
    extern FILE * yyin;

%}

%union {
	int integer;
	char* string;
	struct ArgStruct *Arg;
	struct ASTNode *nptr;
	struct Gsymbol *gptr;
	struct Lsymbol *lptr;
}


%token  BOOL INTEGER AND DOT  NUL ALLOC FREE TYPEDEF DCR INITIALIZE INR MOD OR TRUE FALSE RETURN MAIN DECL ENDDECL BOOLEAN NUM BEGINE ID IF LT decl Enddecl GT LE GE EQ NE END THEN ELSE READ WRITE ENDIF ENDWHILE endif WHILE DO 
%left '+' '-' AND OR MOD
%left '*' '/'
%right DOT
%nonassoc LT GT LE GE EQ NE 
%type <nptr> E 
%type <nptr> Exprlist FIELD
%type <nptr> Retstmt LDefblock
%type <nptr> Body MBlock
%type <nptr> Exphead  Fdef
%type <integer> type type2 typeu type3
%type <nptr> Fdeflist
%type <nptr> stmt
%type <nptr> ID
%type <nptr> NUM NUL
%type <lptr>  lidlist Ldeflist
%type <Arg>  argl  Arg narg
%type <nptr> slist
%type <gptr> idlist
%type <gptr> var
%type <gptr> GDecList Udef UDecList ULIST

%%
Program : Udef Gdef Fdeflist MBlock {};


//User defined Data types starts here

Udef: UDecList {$$=NULL;};
     | {$$=NULL;};

UDecList :UDecList TYPEDEF typeu '{' ULIST '}' {decl_typeu->fields=fieldHead;
                                                fieldHead=NULL;
                                                 no=1;}
         |TYPEDEF typeu '{' ULIST '}' {decl_typeu->fields=fieldHead;
                                       fieldHead=NULL;
                                       no=1;};
 


ULIST : type ID ';' { createField(decl_type->name,$2->name);
                      free($2);
                    }
      | ULIST type ID ';' {createField(decl_type->name,$3->name);
                           free($3);}

type:  ID      {decl_type=getType($1->name);
                 if(decl_type==NULL) {
                  printf("Error:Data type %s\n is not declared\n",$1->name);
                  exit(1);
                 }
 
                 free($1);
               }


type2: ID      {
               decl_type2=getType($1->name);
                if(decl_type2==NULL) {
                  printf("Error:Data type %s\n is not declared\n",$1->name);
                  exit(1);
                 }
                 free($1);
               }

type3: ID      {
               decl_type3=getType($1->name);
                if(decl_type3==NULL) {
                  printf("Error:Data type %s\n is not declared\n",$1->name);
                  exit(1);
                 }
                 free($1);
               }               


typeu: ID {createType($1->name);
	         decl_typeu=getType($1->name);
           free($1);}


//User defined data types ends here

Gdef : DECL GDecList ENDDECL {}; 

GDecList : GDecList GDecL {;} | GDecL {;};


   

//Global declaration part starts here

GDecL : type idlist ';' {}


idlist : idlist ',' var {} 
       | var {};


var : ID '[' NUM ']' {             
            GInstall($1->name,decl_type,$3->val,NULL);
            free($1);
            free($3); 
            }

		    | ID {  
		           GInstall($1->name,decl_type,1,NULL);
		           free($1);
		         }

		    | ID '(' Arg')' {
        
		      	GInstall($1->name,decl_type,0,$3);
		        free($1);
		        Reinitialise();
		    };



//Global declaration part ends here

//argument list

Arg: type2 argl narg {

		struct ArgStruct *temp=$3;
		if(temp) {
			while(temp->next)
			 	temp=temp->next;
		}
		if(temp) {
			temp->next=$2;
			$$=$3;
		}
		else {
			$$=$2;
		}
  }

  | {$$=NULL;}
 
narg : ';' Arg {$$=$2;}
     | {$$=NULL;}

argl : argl ',' ID 
      { 
      	struct ArgStruct *temp=Arginstall($3->name,decl_type2,1);
          temp->next=$1;
          $$=temp;
          free($3);
       }

     | argl ',' '&' ID {
     	 struct ArgStruct *temp=Arginstall($4->name,decl_type2,2);
         temp->next=$1;
         $$=temp;
         free($4);
     }

     | ID {
     	       $$=Arginstall($1->name,decl_type2,1);
     	       free($1);
          }

     | '&' ID { 
               $$=Arginstall($2->name,decl_type2,2);
               free($2);
          }



//function definition list

Fdeflist : {}
         | Fdeflist Fdef {};

Fdef     : type ID '(' Arg ')'  '{' LDefblock Body '}' {
                    
				
	                    validate($2->name,decl_type,$4,$8);
 
                      struct GSymbol *temp=GLookup($2->name);

                      $2->Gentry=temp;

                                   
                      function_gen($2->name,$8);
                     
                      Reinitialise();
                  }

      


LDefblock : DECL Ldeflist ENDDECL 	{ }
          | {};

Ldeflist : Ldeflist LDecL {;} 

           | LDecL {};

           


LDecL : type3 lidlist ';' {}

   
lidlist : lidlist ',' ID {

		   $$=LInstall($3->name,decl_type3,0);
           free($3);	
        }

          | ID  {  
           $$=LInstall($1->name,decl_type3,0);
           free($1);
        };    

   

MBlock: ID MAIN  '(' ')' '{' LDefblock Body '}' {
		    fprintf(fp,"\n\n");
		    $$ = TreeCreate("integer",NODETYPE_MAIN,0,NULL,NULL,$7,NULL,NULL);
			code_gen($$);
	        Reinitialise();
       }

Body : BEGINE slist Retstmt END 
       {
       	    $$=TreeCreate("void",NODETYPE_BODY,0,NULL,NULL,$2,$3,NULL);
       }

slist: slist stmt {$$ = TreeCreate("void", NODETYPE_SLIST , 0,NULL,NULL, $1, $2, NULL);}
      | {$$=NULL;} 
      

stmt :IF '(' E ')' THEN slist ELSE slist endif ';' 
           {
             $$=TreeCreate("void",NODETYPE_IFELSE,0,NULL,NULL,$3,$6,$8);
           } 

      | IF '(' E ')' THEN slist endif ';'    
           {
             $$=TreeCreate("void",NODETYPE_IF,0,NULL,NULL,$3,$6,NULL);
           }


      | ID '=' ALLOC '(' ')' ';' {
            setid($1);
            $$=TreeCreate("void",NODETYPE_ALLOC,0,NULL,NULL,$1,NULL,NULL);
      }   

      | ID '[' E ']' '=' ALLOC '(' ')' ';' {
            setida($1);
            $1->ptr1=$3;
            $$=TreeCreate("void",NODETYPE_ALLOC,0,NULL,NULL,$1,NULL,NULL);
      }     

      | FIELD '=' ALLOC '(' ')' ';' {
            $$=TreeCreate("void",NODETYPE_ALLOC,0,NULL,NULL,$1,NULL,NULL);
      }   

      | FREE '(' ID ')' ';' {
          setid($3);
          $$=TreeCreate("void",NODETYPE_FREE,0,NULL,NULL,$3,NULL,NULL);
      } 

      | FREE '(' ID '[' E ']' ')' ';' {
          setida($3);
          $3->ptr1=$5;
          $$=TreeCreate("void",NODETYPE_FREE,0,NULL,NULL,$3,NULL,NULL);
      }           
          
      | FREE '(' FIELD ')' ';' {
          $$=TreeCreate("void",NODETYPE_FREE,0,NULL,NULL,$3,NULL,NULL);
      }

      | WHILE '(' E ')' DO slist ENDWHILE ';'
           {
            $$=TreeCreate("void",NODETYPE_WHILE,0,NULL,NULL,$3,$6,NULL);
           }

      | READ '(' ID ')' ';' 

           {printf("read\n");
           setid($3);
           $$=TreeCreate("void",NODETYPE_READ,0,NULL,NULL,$3,NULL,NULL);}

      | READ '(' FIELD ')' ';' {
          $$=TreeCreate("void",NODETYPE_READ,0,NULL,NULL,$3,NULL,NULL);        
      }

      | READ '(' ID '[' E ']' ')' ';' {
             setida($3);                 
              $3->ptr1=$5;
              $$=TreeCreate("void",NODETYPE_READ,0,NULL,NULL,$3,NULL,NULL);
              }

      | WRITE '(' E ')' ';'{
              $$=TreeCreate("void",NODETYPE_WRITE,0,NULL,NULL,$3,NULL,NULL);
              }

      

      | INITIALIZE '(' ')' ';' {
         $$=TreeCreate("void",NODETYPE_INITIAL,0,NULL,NULL,NULL,NULL,NULL);
      }

      | ID '=' E ';'       { 
            
            setid($1);
            $$=TreeCreate("void",NODETYPE_ASGN,0,NULL,NULL,$1,$3,NULL);
      
      }

      | ID INR ';' {
        setid($1);
        $$=TreeCreate("void",NODETYPE_INR,0,NULL,NULL,$1,NULL,NULL);
      }

      | FIELD INR ';' {
        if(strcmp($1->type->name,"integer")!=0) {
        printf("Invalid operand for increment\n");
        exit(1);
        } 

        $$=TreeCreate("void",NODETYPE_INR,0,NULL,NULL,$1,NULL,NULL);
      }

      | FIELD DCR ';' {
       if(strcmp($1->type->name,"integer")!=0) {
        printf("Invalid operand for decrement\n");
        exit(1);
        } 

        $$=TreeCreate("void",NODETYPE_DCR,0,NULL,NULL,$1,NULL,NULL);
      }

      | ID DCR ';' {
        setid($1);
        $$=TreeCreate("void",NODETYPE_DCR,0,NULL,NULL,$1,NULL,NULL);  
      }

      | ID '[' E ']' INR ';' {
        setida($1);
        $1->ptr1=$3;
        $$=TreeCreate("void",NODETYPE_INR,0,NULL,NULL,$1,NULL,NULL);
      }

       | ID '[' E ']' DCR ';' {
        setida($1);
        $1->ptr1=$3;
        $$=TreeCreate("void",NODETYPE_DCR,0,NULL,NULL,$1,NULL,NULL);
      }

      | FIELD '=' E ';' {
           $$=TreeCreate("void",NODETYPE_ASGN,0,NULL,NULL,$1,$3,NULL);

      }

      | ID '[' E ']' '=' E  ';' {
            setida($1);
            

            if(strcmp($3->type->name,"integer")!=0) {
                printf("Invalid array\n");
                exit(1);
            }
            $1->ptr1=$3;
            $$=TreeCreate("void",NODETYPE_ASGN,0,NULL,NULL,$1,$6,NULL);
    }; 


Retstmt : RETURN E ';'  { $$ = TreeCreate("void", NODETYPE_RET,0,NULL,NULL,$2,NULL,NULL);
                          };
 
 E : E '+' E  {$$ = TreeCreate("integer",NODETYPE_PLUS,0,NULL,NULL,$1,$3,NULL);}
   | E '-' E {$$ = TreeCreate("integer",NODETYPE_MINUS,0,NULL,NULL,$1,$3,NULL);} 
   | E '*' E {$$ = TreeCreate("integer",NODETYPE_MUL,0,NULL,NULL,$1,$3,NULL);} 
   | E '/' E {$$ = TreeCreate("integer",NODETYPE_DIV,0,NULL,NULL,$1,$3,NULL);} 
   | E GT E  {$$ = TreeCreate("boolean",NODETYPE_GT,0,NULL,NULL,$1,$3,NULL);} 
   | E LT E  {$$ = TreeCreate("boolean",NODETYPE_LT,0,NULL,NULL,$1,$3,NULL);} 
   | E MOD E  {$$ = TreeCreate("integer",NODETYPE_MOD,0,NULL,NULL,$1,$3,NULL);} 
   | E AND E {$$ = TreeCreate("boolean",NODETYPE_AND,0,NULL,NULL,$1,$3,NULL);}
   | E OR E {$$ = TreeCreate("boolean",NODETYPE_OR,0,NULL,NULL,$1,$3,NULL);}
   | E GE E  {$$ = TreeCreate("boolean",NODETYPE_GE,0,NULL,NULL,$1,$3,NULL);}
   | E EQ E  {$$ = TreeCreate("boolean",NODETYPE_EQ,0,NULL,NULL,$1,$3,NULL);}
   | E NE E  {$$ = TreeCreate("boolean",NODETYPE_NE,0,NULL,NULL,$1,$3,NULL);}
   | E LE E  {$$ = TreeCreate("boolean",NODETYPE_LE,0,NULL,NULL,$1,$3,NULL);} 
   | '(' E ')' { 
               $$=$2;
              }
   | FIELD {$$=$1;}
   |  ID '(' Exprlist ')' {
            struct Gsymbol *temp=GLookup($1->name);
                    if(temp==NULL) {
                        printf("Undefined function %s\n",$1->name);
                        exit(1);
                    }
                    else if(temp->size!=0) {
                        printf("Invalid references to a variable %s %d\n",temp->name,temp->size);
                        exit(1);
                    }
                    else {
                        
                      $$=TreeCreate(temp->type->name,NODETYPE_FUNC,0,$1->name,$3,NULL,NULL,NULL);
                    }

                 }


   | ID         {  
                  
                  setid($1); 
                  $$=$1;
                } 


   | ID '[' E ']'  {
              setida($1);
              $1->ptr1=$3;   
              $$=$1;
              }

   | NUM           {
                   $$=$1;
                  }

   | NUL {$$=$1;}


   | '-' NUM     {
      $2->val=$2->val*(-1);
      $$=$2;
   }

   
   | TRUE {  $$= TreeCreate("boolean",NODETYPE_LEAF,TTRUE,NULL,NULL,NULL,NULL,NULL);}
   | FALSE {$$=TreeCreate("boolean",NODETYPE_LEAF,TFALSE,NULL,NULL,NULL,NULL,NULL);}


FIELD     : ID DOT  ID       {
                              setid($1);
                              
                              struct Fieldlist *field=checkfield($1->type,$3->name);
                              tt=field->type;
                              $$=TreeCreate(tt->name,NODETYPE_DOT,0,NULL,NULL,$1,$3,NULL);

                            }  

          | ID '[' E ']' DOT  ID       {
                              setida($1);
                              $1->ptr1=$3;
                              struct Fieldlist *field=checkfield($1->type,$6->name);
                              tt=field->type;
                              $$=TreeCreate(tt->name,NODETYPE_DOT,0,NULL,NULL,$1,$6,NULL);

                            }  

          | FIELD DOT ID   {
                
                 struct Fieldlist *field=checkfield(tt,$3->name);
                 $1->node=NODETYPE_DOTS;
                 tt=field->type;
                 $$=TreeCreate(tt->name,NODETYPE_DOT,0,NULL,NULL,$1,$3,NULL);

          }
          ;


Exprlist : {$$=NULL;}
         | Exphead  {$$=$1;};


Exphead:  Exphead ',' E {
	              $$=TreeCreate("void",NODETYPE_EXPLIST,0,NULL,NULL,$1,$3,NULL);
						}

        | E {$$=TreeCreate("void",NODETYPE_EXPLIST,0,NULL,NULL,NULL,$1,NULL);};
  
%%

int yyerror(char const* msg) {
	printf("%s %s\n",msg,yylval);
  exit(1);
}

void initialise_type() {
	createType("integer");
	createType("boolean");
	createType("void");
	createType("NULL");
}

void initialise_heap() {
  

  fprintf(fp,"LL0:\n");
  fprintf(fp,"MOV [0] 1\n\n");
  fprintf(fp,"MOV R0 1\n");

  
  fprintf(fp,"le5:\n");
  fprintf(fp,"MOV R1 1750\n");
  fprintf(fp,"EQ R1 R0\n");
  fprintf(fp,"JNZ R1,loop6\n");
  fprintf(fp,"MOV [R0] -1\n");
  fprintf(fp,"INR R0\n");
  fprintf(fp,"JMP le5\n");
  fprintf(fp,"loop6:\n");

  fprintf(fp,"MOV R0 1\n");
  fprintf(fp,"loop:\n");
  fprintf(fp,"MOV R1 1585\n");
  fprintf(fp,"MOV R2 R0\n");

  fprintf(fp,"LE R0 R1\n");
  fprintf(fp,"JZ R0,outs\n");
  
  fprintf(fp,"MOV R0 R2\n");
  fprintf(fp,"MOV R1 16\n");
  fprintf(fp,"ADD R1 R0\n");
  fprintf(fp,"MOV [R0] R1\n");
  fprintf(fp,"MOV R0 R1\n");

  fprintf(fp,"JMP loop\n");

  fprintf(fp,"outs:\n");
  fprintf(fp,"RET\n\n");
}

void initalise_free() {
  
  fprintf(fp,"LL1:\n");
  fprintf(fp,"MOV R0 SP\n");
  fprintf(fp,"MOV R1 -1\n");
  fprintf(fp,"ADD R0 R1\n");
  
  fprintf(fp,"MOV R1 R0\n");//setting -1 for pointer
  fprintf(fp,"DCR R1\n");
  fprintf(fp,"MOV R1 [R1]\n");
  fprintf(fp,"MOV [R1] -1\n");

  fprintf(fp,"MOV R0 [R0]\n");
  fprintf(fp,"MOV [R0] [0]\n");
  fprintf(fp,"MOV [0] R0\n");
  fprintf(fp,"INR R0\n");
  
  fprintf(fp,"MOV R1 0\n");//deallocating all fields in chunk
  fprintf(fp,"MOV R2 15\n");
  fprintf(fp,"LOOP:\n");
  fprintf(fp,"MOV R3 R1\n");
  fprintf(fp,"EQ R3 R2\n");
  fprintf(fp,"JNZ R3,LOOP2\n");
  fprintf(fp,"MOV [R0] -1\n");
  fprintf(fp,"INR R0\n");
  fprintf(fp,"INR R1\n");
  fprintf(fp,"JMP LOOP\n");
  fprintf(fp,"LOOP2:\n");
  
  fprintf(fp,"RET\n\n");
}


void initialise_alloc() {
  
  fprintf(fp,"LL2:\n");
  fprintf(fp,"MOV R0 [0]\n");
  fprintf(fp,"MOV [0] [R0]\n");
  fprintf(fp,"MOV R1 SP\n");
  fprintf(fp,"MOV R2 -4\n");
  fprintf(fp,"ADD R1 R2\n");
  fprintf(fp,"MOV [R1] R0\n");
  fprintf(fp,"RET\n\n");
}

int main(int argc,char *argv[]) {
    yyin=fopen(argv[1],"r");
    fp=fopen("out","w");
    initialise_type();
    initialise_heap();
    initalise_free();
   
    Reinitialise();
    initialise_alloc();
	  yyparse();
	  fclose(fp);
	  return 1;
}
