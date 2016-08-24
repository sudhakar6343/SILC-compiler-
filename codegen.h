int code_gen (struct ASTNode * root);
int get_reg();
int get_label();
void free_reg();

FILE *fp;
int reg=-1;
int label=0;