%option noyywrap nounput noinput

%{

#include <cstdlib>
#include <iostream>

%}

%%
. {
  std::cerr << "Lexical error, unknown character: '" << *yytext << "'" << std::endl;
  exit(EXIT_FAILURE);
}

%%
