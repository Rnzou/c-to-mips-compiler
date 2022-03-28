%option noyywrap

%{
  #include "parser.tab.hpp"
%}

D			  [0-9]
L			  [a-zA-Z_]
H			  [a-fA-F0-9]
E			  [Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%%

"int"				                        { return (T_INT); }
"void"							                { return (T_VOID); }
"switch"				                    { return (T_SWITCH); }
"case"                              { return (T_CASE); }
"default"                           { return (T_DEFAULT); }
"double"                            { return (T_DOUBLE); }
"float"                             { return (T_FLOAT); }
"continue"				                  { return (T_CONTINUE); }
"return"			  	                  { return (T_RETURN); }
"if"					                      { return (T_IF); }
"else"					                    { return (T_ELSE); }
"while"					                    { return (T_WHILE); }
"sizeof"                            { return (T_SIZEOF); }
"enum"                              { return (T_ENUM); }
"unsigned"                          { return (T_UNSIGNED); }
"char"                              { return (T_CHAR); }
"for"					                      { return (T_FOR); }
"break"					                    { return (T_BREAK); }

[(]                                 { return (T_LBRACKET); }
[)]                                 { return (T_RBRACKET); }
[[]                                 { return (T_SQUARE_LBRACKET); }
[]]                                 { return (T_SQUARE_RBRACKET); }
[{]                                 { return (T_CURLY_LBRACKET); }
[}]                                 { return (T_CURLY_RBRACKET); }
[:]                                 { return (T_COLON); }
[;]                                 { return (T_SEMICOLON); }
[,]                                 { return (T_COMMA); }
[/]                                 { return (T_DIVIDE); }
[=]                                 { return (T_ASSIGN); }

[*]                                 { return (T_MULTIPLY); }
[+]                                 { return (T_PLUS); }
[-]                                 { return (T_MINUS); }
[%]                                 { return (T_MODULO); }
[>]                                 { return (T_GREATER); }
[<]                                 { return (T_LESS); }
[&]                                 { return (T_BITWISE_AND); }
[|]                                 { return (T_BITWISE_OR); }
[\^]                                { return (T_BITWISE_XOR); }
[=][=]                              { return (T_EQUAL); }
[<][=]				                      { return (T_LESS_EQUAL); }
[>][=]				                      { return (T_GREATER_EQUAL); }
[!][=]				                      { return (T_NOT_EQUAL);	}
[-][=]				                      { return (T_SUB_ASSIGN); }
[+][+]				                      { return (T_INCREMENT); }
[-][-]				                      { return (T_DECREMENT); }
[+][=]				                      { return (T_ADD_ASSIGN); }
[&][&]                              { return (T_LOGICAL_AND); }
[|][|]                              { return (T_LOGICAL_OR); }
[>][>]                              { return (T_RIGHT_SHIFT); }
[<][<]                              { return (T_LEFT_SHIFT); }

{D}+							                  { yylval.int_num   = strtod(yytext, 0);     return (T_INT_CONSTANT); }
[0-9]+[.][0-9]+[f|F|l|L]            { yylval.float_num = strtod(yytext, 0);     return (T_FLOAT_CONSTANT); }
[0-9]+[.][0-9]+                     { yylval.float_num = strtod(yytext, 0);     return (T_DOUBLE_CONSTANT); }
[0-9]+[.][0-9]+[u|U]                { yylval.float_num = strtod(yytext, 0);     return (T_UNSIGNED_CONSTANT); }
{L}({L}|{D})*                       { yylval.string  = new std::string(yytext); return (T_IDENTIFIER); }

[ \t\r\n]+		                      { ; }
.                                   { fprintf(stderr, "Invalid token\n");       exit(1); }

%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse wrong : %s\n", s);
  exit(1);
}

