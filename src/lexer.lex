%option noyywrap nounput noinput

%{

#include <cstdlib>
#include <iostream>
#include <string>

#include "parser.tab.hpp"

%}

%%

"val" return val_token;
"var" return var_token;

[a-zA-Z_][a-zA-Z_0-9]* {
  yylval.string_value = new std::string(yytext);
  return id_token;
}

[0-9]+(\.[0-9]*)? {
  yylval.double_value = atof(yytext);
  return num_token;
}


[=(),;+*/-] return *yytext;

[ \t\n] {}

. {
  std::cerr << "Lexical error, unknown character: '" << *yytext << "'" << std::endl;
  exit(EXIT_FAILURE);
}

%%
