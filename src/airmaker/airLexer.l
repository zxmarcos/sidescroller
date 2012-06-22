%{
#include "parser.hpp"
extern "C" int yywrap();
%}

%option caseless
%%

[ \t\n]
[+-]?[0-9]+		{ yylval=atoi(yytext); return Int; }
"begin"			return Begin;
"action"		return Action;
"clsn1"			return Clsn1;
"clsn2"			return Clsn2;
"clsn1default"	return Clsn1Default;
"clsn2default"	return Clsn2Default;
"Loopstart"		return Loopstart;
"["				return '[';
"]"				return ']';
":"				return ':';
","				return ',';
"="				return '=';
"H"				return HFlip;
"V"				return VFlip;
"VH"			return VHFlip;
"HV"			return VHFlip;
"A"				return Add;
"S"				return Sub;
"AS"[0-9]+"D"[0-9]+	return Add;


%%