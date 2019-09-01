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

    const std::string &getId() const {
        return _id;
    }

private:
    std::string _id;
    std::vector<Param*> _params;
    Type _return_type;
};

class FunctionAST {
public:
    FunctionAST(FunctionPrototypeAST *prototype, ExprAST *body) :
            _prototype(prototype), _body(body) {
    };
    llvm::Function* codegen();

    virtual ~FunctionAST();

private:
    FunctionPrototypeAST* _prototype;
    ExprAST* _body;
};

#endif //KOTLIN_LLVM_STATEMENT_HPP
