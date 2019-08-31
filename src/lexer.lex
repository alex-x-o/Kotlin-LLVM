%option noyywrap nounput noinput

%{

#include <cstdlib>
#include <iostream>
#include <string>
#include "sourcetree/ast.hpp"
#include "sourcetree/statement.hpp"

#include "parser.tab.hpp"

%}

%%

"val" return val_token;
"var" return var_token;
"fun" return fun_token;
"external" return external_token;
"return" return return_token;

"Int" return int_type_token; /* Migrate to actual types in the future? */
"Double" return double_type_token;
"String" return string_type_token;

[a-zA-Z_][a-zA-Z_0-9]* {
  yylval.string_value = new std::string(yytext);
  return id_token;
}

[0-9]+(\.[0-9]*)? {
  yylval.double_value = atof(yytext);
  return num_token;
}

\".+?\" {
    // TODO remove quotemarks
    yylval.string_value = new std::string(yytext);
    return str_token;
}

[-=(),;+*/{}\n:] return *yytext;

[ \t] {}

. {
  std::cerr << "Lexical error, unknown character: '" << *yytext << "'" << std::endl;
  exit(EXIT_FAILURE);
}

%%
