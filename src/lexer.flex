%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"
%}

A                  [a-zA-Z_]
D                  [0-9]
H                  [0-9A-Fa-f]
O                  [0-7]

%%
  /* Arithmetic Operators */
[/]               { return (T_DIVIDE); }
[*]               { return (T_MULTIPLY); }
[+]               { return (T_PLUS); }
[-]               { return (T_MINUS); }
[%]               { return (T_MODULO); }

  /* Bitwise Operators */
[&]               { return (T_BITWISE_AND); }
[|]               { return (T_BITWISE_OR); }
[\^]              { return (T_BITWISE_XOR); }
[<<]              { return (T_BITWISE_SHIFT_LEFT); }
[>>]              { return (T_BITWISE_SHIFT_RIGHT); }

  /* Logical Operators */
[!]               { return (T_LOGICAL_NOT); }
[&&]              { return (T_LOGICAL_AND); }
[||]              { return (T_LOGICAL_OR); }

  /* Comparison */
[==]              { return (T_EQUAL); }
[>]               { return (T_GREATER); }
[<]               { return (T_LESS); }

  /* Assignment */
[=]               { return (T_ASSIGN); }

  /* Characters */
[(]               { return (T_LBRACKET); }
[)]               { return (T_RBRACKET); }
[[]               { return (T_SQUARE_LBRACKET); }
[]]               { return (T_SQUARE_RBRACKET); }
[{]               { return (T_CURLY_LBRACKET); }
[}]               { return (T_CURLY_RBRACKET); }
[:]               { return (T_COLON); }
[;]               { return (T_SEMICOLON); }

  /* Types */
"int"					    { return (T_INT); }
"double"				  { return (T_DOUBLE); }
"float"					  { return (T_FLOAT); }
"char"					  { return (T_CHAR); }
"unsigned"				{ return (T_UNSIGNED); }
"signed"				  { return (T_SIGNED); }         // not in spec
"void"					  { return (T_VOID); }           // not in spec

  /* Structures */
"if"					    { return (T_IF); }
"else"					  { return (T_ELSE); }
"while"					  { return (T_WHILE); }
"for"					    { return (T_FOR); }
"switch"				  { return (T_SWITCH); }
"break"					  { return (T_BREAK); }
"continue"				{ return (T_CONTINUE); }
"return"			  	{ return (T_RETURN); }
"case"					  { return (T_CASE); }           // not in spec
"do"					    { return (T_DO); }             // not in spec

  /* Keywords */
"enum"					  { return (T_ENUM); }
"sizeof"				  { return (T_SIZEOF); }
"struct"				  { return (T_STRUCT); }
"typedef"         { return (T_TYPEDEF); }
"volatile"				{ return (T_VOLATILE); }       // not in spec
"register"				{ return (T_REGISTER); }       // not in spec


{A}({A}|{D})*     { yylval.string=new std::string(yytext);
                    return (T_IDENTIFIER); }

[0-9]+            { yylval.string = new std::string(yytext);
                    return (T_CONSTANT); }

[ \t\r\n]+		    { /* whitespace */ }
.                 { fprintf(stderr, "Invalid token\n"); exit(1); }

%%
[^\]]*

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}