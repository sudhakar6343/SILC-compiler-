%{
	#include "y.tab.h"
   #include "abstracttree.h"
   char * val;
   int value;

%}

letter [a-zA-Z]
number [0-9]

%%

{number}+ {   
              value=atoi(yytext);         
             yylval.nptr= TreeCreate("integer",NODETYPE_LEAF,value,NULL,NULL,NULL,NULL,NULL);
          return  NUM;}


"&&" {return AND;}
[+/*=;(),{}&] {return *yytext;}
"." {return DOT;}
"||" {return OR;}
"++" {return INR;}
"--" {return DCR;}
 "%" {return MOD;}
"typedef" {return TYPEDEF;}
"-" {return *yytext;}
"==" {return EQ;}
">" {return GT;}
"<" {return LT;}
">=" {return GE;}
"<=" {return LE;}
"!=" {return NE;}
"null" {yylval.nptr= TreeCreate("NULL",NODETYPE_LEAF,-1,NULL,NULL,NULL,NULL,NULL);return NUL;}
"end" {return END;}
"["   {return *yytext;}
"]"   {return *yytext;}
"TRUE" {return TRUE;}
"FALSE" {return FALSE;}
 "endwhile" {return ENDWHILE;}
"if"       {return IF;}
"else"     {return ELSE;}
"free"  {return FREE;}
"then"     {return THEN;}
"while"    {return WHILE;}
"endif"    {return endif;}
"alloc"  {return ALLOC;}
"read"     {return READ;}
"main"     {return MAIN;}
"write"    {return WRITE;}
"initialize" {return INITIALIZE;}
"do"       {return DO;}
"return"    {return RETURN;}
"begin"    {return BEGINE;}
"decl"        {return DECL;}
"enddecl"     {return ENDDECL;}
"\n" {}
" " {}

{letter}({letter}|{number})* { 
                          val=yytext;	                         
                           yylval.nptr=TreeCreate("void",NODETYPE_ID,0,val,NULL,NULL,NULL,NULL);
                           return ID;}
.          {}



%%

int yywrap() {
	return 1;
}
