%{
#include <stdio.h>
#include "../airloader.h"
#include "../system.h"
#define YYTYPE int

extern int yylex();
AIRLoader *g_animParser = NULL;
extern char *yytext;

void yyerror(const char *fmt) {
Log("Error %s\n%s\n", fmt, yytext);
}
extern "C" int yywrap()
{
return 1;
}

%}

%token '['
%token ']'
%token '='
%token ':'
%token ','
%token Begin
%token Action
%token Int
%token Clsn1
%token Clsn2
%token Clsn1Default
%token Clsn2Default
%token Loopstart
%token HFlip
%token VFlip
%token VHFlip
%token Add
%token Sub


%%

air:
   | air stmt
   ;
   
stmt:'[' Begin Action Int ']' { g_animParser->createAction($4); }
	| Int ',' Int ',' Int ',' Int ',' Int  { g_animParser->createFrame($1,$3,$5,$7,$9); }
	| Int ',' Int ',' Int ',' Int ',' Int ',' HFlip  { g_animParser->createFrame($1,$3,$5,$7,$9); }
	| Int ',' Int ',' Int ',' Int ',' Int ',' VFlip  { g_animParser->createFrame($1,$3,$5,$7,$9); }
	| Int ',' Int ',' Int ',' Int ',' Int ',' VHFlip  { g_animParser->createFrame($1,$3,$5,$7,$9); }
	| Int ',' Int ',' Int ',' Int ',' Int ',' VHFlip ',' Add { g_animParser->createFrame($1,$3,$5,$7,$9); }
	| Int ',' Int ',' Int ',' Int ',' Int ',' VHFlip ',' Sub { g_animParser->createFrame($1,$3,$5,$7,$9); }
	| Int ',' Int ',' Int ',' Int ',' Int ',' ',' Add  { g_animParser->createFrame($1,$3,$5,$7,$9); }
	| Int ',' Int ',' Int ',' Int ',' Int ',' ',' Sub  { g_animParser->createFrame($1,$3,$5,$7,$9); }
	| Clsn1Default ':' Int { g_animParser->createClsnList(1, true, $3); }
	| Clsn2Default ':' Int { g_animParser->createClsnList(2, true, $3); }
	| Clsn1 ':' Int { g_animParser->createClsnList(1, false, $3); }
	| Clsn2 ':' Int { g_animParser->createClsnList(2, false, $3); }
	| Clsn1 '[' Int ']' '=' Int ',' Int ',' Int ',' Int { g_animParser->createClsn(1, $3, $6, $8, $10, $12); }
	| Clsn2 '[' Int ']' '=' Int ',' Int ',' Int ',' Int { g_animParser->createClsn(2, $3, $6, $8, $10, $12); }
	| Loopstart { g_animParser->setLoopstart(); }
	;
	
%%