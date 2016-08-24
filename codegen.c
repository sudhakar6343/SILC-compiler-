int get_reg() {

  if(reg<7) {
    reg++;
    return reg;
  }
  else {
    printf("Register Memory exceed\n");
    exit(1);
  }
}

void free_all_regs () {
  reg=-1;
}

void free_reg() {
  if(reg==-1) {
    printf("register error\n");
    exit(1);
  }
  
   
  
  reg--;
}

int get_label() {
   label++;
   return label;
}

void userextra(struct ASTNode *temp,int r) {
  if(temp->node==NODETYPE_DOT) 
          fprintf(fp,"MOV R%d [R%d]\n",r,r);
}

int functioncall_gen(struct ASTNode *root) {

  int r,r1,r2,c=0;
  struct Gsymbol *g=GLookup(root->name);
  struct ArgStruct *list=g->arglist;
  struct ASTNode *templist=root->arglist;

  //Saving the values in the registers;
  
  while(c<=reg) {
    fprintf(fp,"PUSH R%d\n",c);
    c++;
  }

  c--;


  while(templist) {

    if(list->passType==1) {
       
      r=code_gen(templist->ptr2);
      if(templist->ptr2->node==NODETYPE_DOT)
        fprintf(fp,"MOV R%d [R%d]\n",r,r);
      fprintf(fp,"PUSH R%d\n",r);
       
      free_reg();
    }

    else {

      if(templist->ptr2->node==NODETYPE_DOT) {
        r=code_gen(templist->ptr2);

        fprintf(fp,"PUSH R%d\n",r);
       
        free_reg();
      }

      else if(templist->ptr2->Lentry) {
        
        r1=get_reg();
        r2=get_reg();
        fprintf(fp,"MOV R%d BP\n",r1);
        fprintf(fp,"MOV R%d %d\n",r2,templist->ptr2->Lentry->binding);
        fprintf(fp,"ADD R%d R%d\n",r1,r2);
        free_reg();

        if(templist->ptr2->Lentry->ref==2) {
          fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
        }  
        fprintf(fp,"PUSH R%d\n",r1);
        free_reg();
      }
      
      else {    
         r=get_reg();
         fprintf(fp,"MOV R%d %d\n",r,templist->ptr2->Gentry->binding);
         if(templist->ptr2->ptr1) {
         	r2=code_gen(templist->ptr2->ptr1);
         	fprintf(fp,"ADD R%d R%d\n",r,r2);
         	free_reg();
         }
         fprintf(fp,"PUSH R%d\n",r);
         free_reg();
      }
      
    }

    templist=templist->ptr1;
    list=list->next;
  }

 
  r=get_reg();                   //space for return value
  fprintf(fp,"PUSH R%d\n",r);
  free_reg();

  fprintf(fp,"CALL LL%d\n",g->binding);
  reg=c;
  r=get_reg();
  fprintf(fp,"POP R%d\n",r);  //retriving the return value

  r1=get_reg();
  list=g->arglist;
  while(list) {
    fprintf(fp,"POP R%d\n",r1);
    list=list->next;
  }
  free_reg();
  //Restoring the register values
  
  
  while(c>=0) {
    fprintf(fp,"POP R%d\n",c);
    c--;
  }

  return r;
}

void function_gen(char *name,struct ASTNode *body) {

  int r1;
  struct Gsymbol *g=GLookup(name);   
  fprintf(fp,"LL%d:\n",g->binding);
  fprintf(fp,"PUSH BP\n");
  fprintf(fp,"MOV BP SP\n");
  struct Lsymbol *temp=LSymbolHead;
  free_all_regs();
  r1=get_reg();
  
  
  while(temp) {
    if(temp->ref==0) {
      fprintf(fp,"PUSH R%d\n",r1);
    }
    temp=temp->next;
  }
  free_reg();

  code_gen(body);
  
  r1=get_reg();
  temp=LSymbolHead;

  while(temp) {
    if(temp->ref==0) {
      fprintf(fp,"POP R%d\n",r1);
    }
    temp=temp->next;
  }
  free_reg();
  free_all_regs();
  fprintf(fp,"POP BP\n");
  fprintf(fp,"RET\n");
}



int code_gen (struct ASTNode * root) {
  int l1,l2,r,r1,r2,r3,bind;
  struct Fieldlist *field;
    if(!root) 
      return 0;
    
      switch(root->node) {

      
          case NODETYPE_MUL :

                r1=code_gen(root->ptr1);
                userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                 userextra(root->ptr2,r2);

                fprintf(fp,"MUL R%d R%d\n",r1,r2);
                free_reg();
                return r1;

           case NODETYPE_PLUS :
                r1=code_gen(root->ptr1);
                 userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                 userextra(root->ptr2,r2);

                fprintf(fp,"ADD R%d R%d\n",r1,r2);
                free_reg();
                return r1;

           
            case NODETYPE_MINUS :
                r1=code_gen(root->ptr1);
              userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                userextra(root->ptr2,r2);

                fprintf(fp,"SUB R%d R%d\n",r1,r2);
                free_reg();
                return r1;

            case NODETYPE_MOD :
                r1=code_gen(root->ptr1);
                 userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                 userextra(root->ptr2,r2);

                fprintf(fp,"MOD R%d R%d\n",r1,r2);

                free_reg();
                return r1;    

            case NODETYPE_DIV :
                r1=code_gen(root->ptr1);
                 userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                 userextra(root->ptr2,r2);

                fprintf(fp,"DIV R%d R%d\n",r1,r2);
                free_reg();
                return r1;
            
            
            case NODETYPE_DOT:
                r1=code_gen(root->ptr1);
                field=checkfield(root->ptr1->type,root->ptr2->name);
                r2=get_reg();
                fprintf(fp,"MOV R%d %d\n",r2,field->no);
                fprintf(fp,"ADD R%d R%d\n",r1,r2);
               
                free_reg();
                return r1;
               
           case NODETYPE_DOTS:
                r1=code_gen(root->ptr1);
                field=checkfield(root->ptr1->type,root->ptr2->name);
                r2=get_reg();
                fprintf(fp,"MOV R%d %d\n",r2,field->no);
                fprintf(fp,"ADD R%d R%d\n",r1,r2);
                fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                
      
                free_reg();
                return r1;


            case NODETYPE_MAIN:
             // printf("main\n");
              fprintf(fp,"START\n");
              fprintf(fp,"MOV SP 1750\n");
              fprintf(fp,"MOV BP SP\n");
              r1 = get_reg();
              struct Lsymbol *Ltemp = LSymbolHead;
              while(Ltemp != NULL){
                  if(Ltemp->ref==0){

                    fprintf(fp,"PUSH R%d\n",r1); //allocating memory for each local varaiable
                    Ltemp = Ltemp->next;
                  } 
              }
              fprintf(fp,"MAIN:\n");
              free_reg();
              code_gen(root->ptr1);
              fprintf(fp,"HALT\n");
              return 0;

        
              case NODETYPE_ALLOC:
                
                 if(root->ptr1->node==NODETYPE_DOT) {
                    r1=code_gen(root->ptr1);
                    fprintf(fp,"PUSH R0\n");
                    fprintf(fp,"PUSH R0\nPUSH R1\nPUSH R2\n");
                    fprintf(fp,"CALL LL2\n");
                    fprintf(fp,"POP R2\nPOP R1\nPOP R0\n");  
                    r2=get_reg();
                    fprintf(fp,"POP R%d\n",r2);
                    fprintf(fp,"MOV [R%d] R%d\n",r1,r2);
                    free_reg();
                    free_reg();
                 }
                 else {
                      r1=get_reg();
                      r2=get_reg();
                    if(root->ptr1->Lentry) {
                     
                      fprintf(fp,"MOV R%d BP\n",r1);
                      fprintf(fp,"MOV R%d %d\n",r2,root->ptr1->Lentry->binding);
                      fprintf(fp,"ADD R%d R%d\n",r1,r2);
                      if(root->ptr1->Lentry->ref==2)
                        fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                      
                    }

                    else {

                      fprintf(fp,"MOV R%d %d\n",r1,root->ptr1->Gentry->binding);
                      if(root->ptr1->ptr1) {
                      r2=code_gen(root->ptr1->ptr1);
                      fprintf(fp,"ADD R%d R%d\n",r1,r2);}

                      
                    }

                      fprintf(fp,"PUSH R0\n");
                      fprintf(fp,"PUSH R0\nPUSH R1\nPUSH R2\n");
                    
                      fprintf(fp,"CALL LL2\n"); 
                      fprintf(fp,"POP R2\nPOP R1\nPOP R0\n");   
              
                      fprintf(fp,"POP R%d\n",r2);
                      fprintf(fp,"MOV [R%d] R%d\n ",r1,r2);
                      free_reg();
                      free_reg();

                 }


              return 0;

              case NODETYPE_INITIAL:
                  fprintf(fp,"PUSH R0\nPUSH R1\nPUSH R2\n");
                  fprintf(fp,"CALL LL0\n");
                  fprintf(fp,"POP R2\nPOP R1\nPOP R0\n");
              return 0;


              case NODETYPE_FREE:
                  if(root->ptr1->node==NODETYPE_DOT) {

                      r1=code_gen(root->ptr1);
                    
                      fprintf(fp,"PUSH R0\n");
                      fprintf(fp,"PUSH R1\nPUSH R2\nPUSH R3\n");
                      fprintf(fp,"PUSH R%d\n",r1);
                      fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                      fprintf(fp,"PUSH R%d\n",r1);
                    
                      fprintf(fp,"CALL LL1\n");
                      fprintf(fp,"POP R%d\n",r1);
                      fprintf(fp,"POP R%d\n",r1);
                      fprintf(fp,"POP R3\nPOP R2\nPOP R1\nPOP R0\n");

                      free_reg();
                    

                  }

                 else {
                      r1=get_reg();
                      r2=get_reg();
                
                    if(root->ptr1->Lentry) {
                    
                      fprintf(fp,"MOV R%d BP\n",r1);
                      fprintf(fp,"MOV R%d %d\n",r2,root->ptr1->Lentry->binding);
                      fprintf(fp,"ADD R%d R%d\n",r1,r2);
                      if(root->ptr1->Lentry->ref==2) {
                        fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                      }

                    }

                    else {
                       
                      fprintf(fp,"MOV R%d %d\n",r1,root->ptr1->Gentry->binding);
                      if(root->ptr1->ptr1) {
                      r2=code_gen(root->ptr1->ptr1);
                      fprintf(fp,"ADD R%d R%d\n",r1,r2);}

                      
                    }

                      fprintf(fp,"PUSH R0\n");
                      fprintf(fp,"PUSH R1\nPUSH R2\nPUSH R3\n");
                      fprintf(fp,"PUSH R%d\n",r1);
                      fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                      fprintf(fp,"PUSH R%d\n",r1);
                    
                      fprintf(fp,"CALL LL1\n");
                      fprintf(fp,"POP R%d\n",r1);
                      fprintf(fp,"POP R%d\n",r1);
                      fprintf(fp,"POP R3\nPOP R2\nPOP R1\nPOP R0\n");

                      free_reg();
                      free_reg();
                 }
                 


              return 0;

              case NODETYPE_LT  :
                //  printf("less than\n");
                r1=code_gen(root->ptr1);
                 userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                 userextra(root->ptr2,r2);
                fprintf(fp,"LT R%d R%d\n",r1,r2);
                free_reg();
                return r1;


              case NODETYPE_FUNC:
            ;
                fprintf(fp," // Before funtion call\n\n\n");
                r=functioncall_gen(root);
                return r;

              case NODETYPE_LEAF:
               //printf("leaf\n");
                r1=get_reg();
                fprintf(fp,"MOV R%d %d\n",r1,root->val);
                return r1;

            case NODETYPE_LE  :
               // printf("le\n");
                r1=code_gen(root->ptr1);
                userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                userextra(root->ptr2,r2);
                fprintf(fp,"LE R%d R%d\n",r1,r2);
                free_reg();
                return r1;
            
            case NODETYPE_AND:
                r1 = code_gen(root->ptr1);
                userextra(root->ptr1,r1);
                r2 = code_gen(root->ptr2);
                userextra(root->ptr2,r2);        
                fprintf(fp,"ADD R%d R%d\n",r1,r2);
                fprintf(fp,"MOV R%d 2\n",r2);
                fprintf(fp,"EQ R%d R%d\n",r1,r2);
                free_reg();
                return r1;                 
                break;

        case NODETYPE_OR:
                r1 = code_gen(root->ptr1);
                userextra(root->ptr1,r1);
                r2 = code_gen(root->ptr2);
                userextra(root->ptr2,r2);        
                fprintf(fp,"ADD R%d R%d\n",r1,r2);
                fprintf(fp,"MOV R%d 0\n",r2);
                fprintf(fp,"GE R%d R%d\n",r1,r2);
                free_reg();
                return r1;                 
                break;
                

            case NODETYPE_GE  :
                
                r1=code_gen(root->ptr1);
                userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                userextra(root->ptr2,r2);
                fprintf(fp,"GE R%d R%d\n",r1,r2);
                free_reg();
                return r1;

             case NODETYPE_EQ :
               //printf("eq\n");
                r1=code_gen(root->ptr1);
                userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                userextra(root->ptr2,r2);
                fprintf(fp,"EQ R%d R%d\n",r1,r2);
                free_reg();
                return r1;

            case NODETYPE_NE  :
                
                r1=code_gen(root->ptr1);
                userextra(root->ptr1,r1);
               
                r2=code_gen(root->ptr2);
                userextra(root->ptr2,r2);
                
                fprintf(fp,"NE R%d R%d\n",r1,r2);
                free_reg();
                return r1;

             case NODETYPE_GT :
               
                r1=code_gen(root->ptr1);
                userextra(root->ptr1,r1);
                r2=code_gen(root->ptr2);
                userextra(root->ptr2,r2);
                fprintf(fp,"GT R%d R%d\n",r1,r2);
                free_reg();
                return r1; 
            
                case NODETYPE_ID:
                r1=get_reg();     
               if(root->ptr1==NULL) {

                   if(root->Lentry) {
                    bind=root->Lentry->binding;
               
                    r2=get_reg();
                    fprintf(fp,"MOV R%d BP\n",r1);
                    fprintf(fp,"MOV R%d %d\n",r2,bind);
                    fprintf(fp,"ADD R%d R%d\n",r1,r2);
                    
                    
                    fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                
                    if(root->Lentry->ref==2){
                      fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                    } 

                    
                    free_reg();
                    return r1;
                   }
                   else {
                     bind=root->Gentry->binding;
                     
                     r2=get_reg();
                     fprintf(fp,"MOV R%d %d\n",r2,bind);
                     fprintf(fp,"MOV R%d [R%d]\n",r1,r2);
      

                     free_reg();
                     return r1;
                   }

               }

               else {
                     bind=root->Gentry->binding;
                     r2=code_gen(root->ptr1);
                     fprintf(fp,"MOV R%d %d\n",r1,bind);
                     fprintf(fp,"ADD R%d R%d\n",r1,r2);
                     free_reg();
                     fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                     return r1;
               }
              break;



  
            case NODETYPE_BODY:
             //printf("body\n");
             code_gen(root->ptr1);
             code_gen(root->ptr2);
             break;

             case NODETYPE_DCR:
             case NODETYPE_INR:

                if(root->ptr1->node==NODETYPE_DOT) {
                  r1=code_gen(root->ptr1);
                  r2=get_reg();
                  fprintf(fp,"MOV R%d R%d\n",r2,r1);
                  fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                  if(root->node==NODETYPE_INR)
                    fprintf(fp,"INR R%d\n",r1);
                  else
                    fprintf(fp,"DCR R%d\n",r1);
                  fprintf(fp,"MOV [R%d] R%d",r2,r1);
                  free_reg();
                  free_reg();
                  return 0;
                }

                if(root->ptr1->ptr1) {
            
                    bind=root->ptr1->Gentry->binding;
                    r1=code_gen(root->ptr1->ptr1);
                    r3=get_reg();
                    r2=get_reg();
                    fprintf(fp,"MOV R%d 1\n",r2);
                    fprintf(fp,"MOV R%d %d\n",r3,bind);
                    fprintf(fp,"ADD R%d R%d\n",r1,r3);
                    fprintf(fp,"MOV R%d [R%d]\n",r3,r1);
                    if(root->node==NODETYPE_INR) 
                    fprintf(fp,"INR R%d\n ",r3);
                    else
                    fprintf(fp,"DCR R%d\n ",r3);
                    fprintf(fp,"MOV [R%d] R%d\n",r1,r3);
                    free_reg();
                    free_reg();
                    free_reg();
                  }
                  else {
                    if(root->ptr1->Lentry) {
                    
                       bind=root->ptr1->Lentry->binding;
                       r1=get_reg();
                       r3=get_reg();
                       fprintf(fp,"MOV R%d BP\n",r1);
                       fprintf(fp,"MOV R%d %d\n",r3,bind);
                       fprintf(fp,"ADD R%d R%d\n",r1,r3);
                       if(root->ptr1->Lentry->ref==2)
                       fprintf(fp,"MOV R%d [R%d] \n",r1,r1);
                       fprintf(fp,"MOV R%d [R%d]\n",r3,r1);
                       if(root->node==NODETYPE_INR) 
                       fprintf(fp,"INR R%d\n",r3);
                       else
                        fprintf(fp,"DCR R%d\n ",r3);
                       fprintf(fp,"MOV [R%d] R%d\n",r1,r3);
                       free_reg();
                       free_reg();
                  }
                  else {
                    bind=root->ptr1->Gentry->binding;
                    fprintf(fp,"MOV [%d] R%d\n",bind,r2);
                    free_reg();
                  }

                  }


              break;

             case NODETYPE_ASGN:
                //printf("asg %d  %s\n\n",root->ptr2->node,root->ptr2->name);
                r2=code_gen(root->ptr2);
                //printf("asg2\n");
                if(root->ptr2->node==NODETYPE_DOT) {
                  fprintf(fp,"MOV R%d [R%d]\n",r2,r2);
                }

              if(root->ptr1->node!=NODETYPE_DOT) {
                if(root->ptr1->ptr1) {
                  //  printf("Asgn1\n");
                    bind=root->ptr1->Gentry->binding;
                    r1=code_gen(root->ptr1->ptr1);
                    r3=get_reg();
                    fprintf(fp,"MOV R%d %d\n",r3,bind);
                    fprintf(fp,"ADD R%d R%d\n",r1,r3);
                    fprintf(fp,"MOV [R%d] R%d\n ",r1,r2);
                    free_reg();
                    free_reg();
                    free_reg();
                  
                }else {
                  if(root->ptr1->Lentry) {
                
                     bind=root->ptr1->Lentry->binding;
                     r1=get_reg();
                     r3=get_reg();
                     fprintf(fp,"MOV R%d BP\n",r1);
                     fprintf(fp,"MOV R%d %d\n",r3,bind);
                     fprintf(fp,"ADD R%d R%d\n",r1,r3);
                     if(root->ptr1->Lentry->ref==2)
                     fprintf(fp,"MOV R%d [R%d] \n",r1,r1);
                     free_reg();
                     fprintf(fp,"MOV [R%d] R%d\n",r1,r2);
                     
                     free_reg();
                     free_reg();
                  }
                  else {
                    bind=root->ptr1->Gentry->binding;
                    fprintf(fp,"MOV [%d] R%d\n ",bind,r2);
                    free_reg();
                  }
                }
              }


                else {
                    r1=code_gen(root->ptr1);
                    fprintf(fp,"MOV [R%d] R%d\n  ",r1,r2);
                    free_reg();
                    free_reg();
                }

                return 0;
                break;

          case NODETYPE_READ:
        
              if(root->ptr1->node==NODETYPE_DOT) {
                r1=code_gen(root->ptr1);
                r2=get_reg();
                fprintf(fp,"IN R%d\n",r2);
                fprintf(fp,"MOV [R%d] R%d\n",r1,r2);
                free_reg();
                free_reg();
                return 0;
              }

              if(root->ptr1->Lentry) {
                r1=get_reg();
                r2=get_reg();
              
                fprintf(fp,"MOV R%d BP\n",r1);
                bind=root->ptr1->Lentry->binding;
                fprintf(fp,"MOV R%d %d\n",r2,bind);
                fprintf(fp,"ADD R%d R%d\n",r1,r2);
                fprintf(fp,"IN R%d\n",r2);
                if(root->ptr1->Lentry->ref == 2){
                  fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
                }
                fprintf(fp,"MOV [R%d] R%d\n",r1,r2);
                free_reg();
                free_reg();
              }
              else {
                if(root->ptr1->ptr1) {
                  bind=root->ptr1->Gentry->binding;
                  r3=get_reg();
                  fprintf(fp,"MOV R%d %d\n",r3,bind);
                  r1=code_gen(root->ptr1->ptr1);
                  fprintf(fp,"ADD R%d R%d\n",r3,r1);
                  r2=get_reg();
                  fprintf(fp,"IN R%d\n",r2);
                  fprintf(fp,"MOV [R%d] R%d\n",r3,r2);
                  free_reg();
                  free_reg();
                  free_reg();
                }
                else {
                  bind=root->ptr1->Gentry->binding;
                  r1=get_reg();

                  fprintf(fp,"MOV R%d %d\n",r1,bind);
                  
                  r2=get_reg();
                  fprintf(fp,"IN R%d\n",r2);
                  fprintf(fp,"MOV [R%d] R%d\n",r1,r2);
                  free_reg();
                  free_reg();
                }
              }
              return 0;
              break;

        case NODETYPE_WRITE:
               //printf("write\n");
              r1=code_gen(root->ptr1);
              if(root->ptr1->node==NODETYPE_DOT) 
                fprintf(fp,"MOV R%d [R%d]\n",r1,r1);
              
              fprintf(fp,"OUT R%d\n",r1);
              free_reg();
              return 0;

         case NODETYPE_RET:
                //printf("ret\n");
                r=code_gen(root->ptr1);
                if(root->ptr1->node==NODETYPE_DOT)
                  fprintf(fp,"MOV R%d [R%d]\n",r,r);
                r1=get_reg();
                fprintf(fp,"MOV R%d BP\n",r1);
                r2=get_reg();
                fprintf(fp,"MOV R%d 2\n",r2);
                fprintf(fp,"SUB R%d R%d\n",r1,r2);
                free_reg();
                fprintf(fp,"MOV [R%d] R%d\n",r1,r);
                free_reg();
                free_reg();
                break;

        case NODETYPE_WHILE :
            //  printf("while\n");
              l1=get_label();
              l2=get_label();
              fprintf(fp,"\n//while\n");
              fprintf(fp,"label%d:\n",l1);
              r1=code_gen(root->ptr1);              
              fprintf(fp,"JZ R%d,label%d\n",r1,l2);
              free_reg();
              code_gen(root->ptr2);
              fprintf(fp,"JMP label%d\n",l1);
              fprintf(fp,"\n\n");
              fprintf(fp,"label%d:\n",l2);
              break;
              
      case NODETYPE_IF :

             // printf("if\n");
              fprintf(fp,"\n\n//if\n");
              l1=get_label();
              r1=code_gen(root->ptr1);
              fprintf(fp,"JZ R%d label%d\n",r1,l1);
              free_reg();
              code_gen(root->ptr2);
              fprintf(fp,"label%d:\n",l1);
              break;

               
         case NODETYPE_IFELSE :
              fprintf(fp, "\n\n //ifelse\n\n");
              l1=get_label();
              l2=get_label();
              r1=code_gen(root->ptr1);

            
              fprintf(fp,"JZ R%d label%d\n",r1,l1);
              free_reg();
              code_gen(root->ptr2);

              fprintf(fp,"JMP label%d\n",l2);
              fprintf(fp,"label%d:\n",l1);

              code_gen(root->ptr3);
              fprintf(fp,"label%d:\n",l2);
              break;

        case NODETYPE_SLIST :
             // printf("slist\n");
              code_gen(root->ptr1);
              code_gen(root->ptr2);
              break;
          
  }
}
         
      