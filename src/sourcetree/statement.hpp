#ifndef KOTLIN_LLVM_STATEMENT_HPP
#define KOTLIN_LLVM_STATEMENT_HPP

#include <vector>
#include <string>

#include "sourcetree/ast.hpp"

#include "llvm/IR/Value.h"

class FunctionPrototypeAST {
public:
    FunctionPrototypeAST(std::string id, std::vector<Param*> params, Type return_type) :
            _id(std::move(id)), _params(std::move(params)), _return_type(return_type) {};
    llvm::Function* codegen();
private:
    std::string _id;
    std::vector<Param*> _params;
    Type _return_type;
};

class FunctionAST {
public:
    FunctionAST(std::string id, std::vector<Param*> params, ExprAST* body, Type return_type) :
            _id(std::move(id)), _params(std::move(params)), _body(body), _return_type(return_type)  {};
    llvm::Function* codegen();

private:
    std::string _id;
    std::vector<Param*> _params;
    ExprAST* _body;
    Type _return_type;
};

#endif //KOTLIN_LLVM_STATEMENT_HPP
