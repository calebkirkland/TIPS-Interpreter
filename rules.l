/*********************************************************************
  Name: Caleb Kirkland             NetID: cok18
  Course: CSE 4714              
  Project Part 1 - Lexical Analyzer 
  Purpose of File: Contains the rules for the lexical analyzer
  Uses regular expressions to catch keywords. 
**********************************************************************/
%option yylineno
%option noyywrap
%{
#include "lexer.h"

 /*Global variable to hold the current line number beaing read*/
int line_number = 1;
%}
%%
 /*Keywords*/
"BEGIN"             {return TOK_BEGIN;}
"BREAK"             {return TOK_BREAK;}
"CONTINUE"          {return TOK_CONTINUE;}
"DOWNTO"            {return TOK_DOWNTO;}
"ELSE"              {return TOK_ELSE;}  
"END"               {return TOK_END;}
"FOR"               {return TOK_FOR;}
"IF"                {return TOK_IF;} 
"LET"               {return TOK_LET;}  
"PROGRAM"           {return TOK_PROGRAM;}
"READ"              {return TOK_READ;}
"THEN"              {return TOK_THEN;}
"TO"                {return TOK_TO;}   
"VAR"               {return TOK_VAR;}
"WHILE"             {return TOK_WHILE;}
"WRITE"             {return TOK_WRITE;}     

 /* Datatype Specifiers*/
"REAL"              {return TOK_REAL;}
"INTEGER"           {return TOK_INTEGER;}

 /*Punctuation*/
";"                 {return TOK_SEMICOLON;}
":"                 {return TOK_COLON;}
"("                 {return TOK_OPENPAREN;}
")"                 {return TOK_CLOSEPAREN;}	
"{"                 {return TOK_OPENBRACE;}
"}"                 {return TOK_CLOSEBRACE;}

 /*Operators*/
"+"                 {return TOK_PLUS;}
"-"                 {return TOK_MINUS;}
"*"                 {return TOK_MULTIPLY;}
"/"                 {return TOK_DIVIDE;}
":="                {return TOK_ASSIGN;}
"="                 {return TOK_EQUALTO;}
"<"                 {return TOK_LESSTHAN;}
">"                 {return TOK_GREATERTHAN;}
"<>"                {return TOK_NOTEQUALTO;}
"MOD"               {return TOK_MOD;}
"NOT"               {return TOK_NOT;}
"OR"                {return TOK_OR;}
"AND"               {return TOK_AND;}

 /*Useful Abstractions*/
[A-Z][0-9A-Z]{0,7}  {return TOK_IDENT;}
[0-9]+              {return TOK_INTLIT;}
[0-9]+"."[0-9]+     {return TOK_FLOATLIT;}
'[^'^\n]{0,78}'     {return TOK_STRINGLIT;}

 /*To eat any whitespace and update the line number counter every time there is a \n*/
[\n]                {line_number++;}
[ \t\r]+

[A-Z][0-9A-Z]{8,}   {return TOK_UNKNOWN;} /*Found an identifier with a length above 8*/
'[^']{79,}'         {return TOK_UNKNOWN;} /*Found a string literal with length above 80*/
.                   {return TOK_UNKNOWN;} /*Found an unknown character*/

 /* Recognize end of file */
<<EOF>>            { return TOK_EOF; }


