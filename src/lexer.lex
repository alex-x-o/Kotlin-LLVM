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
"if" return if_token;
"else" return else_token;
"println" return print_token;
[\.\.] return range_token;
"<=" return le_token;
">=" return ge_token;
"+=" return pa_token;
"-=" return ma_token;
"*=" return ta_token;
"/=" return da_token;
"%=" return moda_token;
"true" {
    yylval.boolean_value = true;
    return boolean_token;
}
"false" {
    yylval.boolean_value = false;
    return boolean_token;
}

"Int" return int_type_token; /* Migrate to actual types in the future? */
"Double" return double_type_token;
"String" return string_type_token;

[a-zA-Z_][a-zA-Z_0-9]* {
  yylval.string_value = new std::string(yytext);
  return id_token;
}

[1-9][0-9]* {
  yylval.int_value = atoi(yytext);
  return int_token;
}

[1-9][0-9]*(\.[0-9]*)? {
  yylval.double_value = atof(yytext);
  return double_token;
}

\".+?\" {
    // TODO remove quotemarks
    yylval.string_value = new std::string(yytext);
    return str_token;
}

[-=(),;%+*/<>{}\n:] return *yytext;

[ \t] {}

. {
  std::cerr << "Lexical error, unknown character: '" << *yytext << "'" << std::endl;
  exit(EXIT_FAILURE);
}

%%
