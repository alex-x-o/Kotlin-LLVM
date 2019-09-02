#ifndef KOTLIN_LLVM_STATEMENT_HPP
#define KOTLIN_LLVM_STATEMENT_HPP

#include <utility>
#include <vector>
#include <string>

#include "sourcetree/ast.hpp"

#include "llvm/IR/Value.h"

class Statement {
public:
    virtual ~Statement() = default;
    virtual void codegen() = 0;
};

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

class FunctionAST : public Statement {
public:
    FunctionAST(FunctionPrototypeAST *prototype, std::vector<Statement*> *body) :
            _prototype(prototype), _body(body) {
    };
    void codegen() override;

    ~FunctionAST() override;

private:
    FunctionPrototypeAST* _prototype;
    std::vector<Statement*>* _body;
};

class ExternalFunctionStatement : public Statement {
public:
    explicit ExternalFunctionStatement(FunctionPrototypeAST* prototype) : _prototype(prototype) {};
    void codegen() override;
    ~ExternalFunctionStatement() override;

private:
    FunctionPrototypeAST* _prototype;
};

class ExpressionStatement : public Statement {
public:
    explicit ExpressionStatement(ExprAST *expr) : expr(expr) {};

    void codegen() override {
        expr->codegen();
    }

    ~ExpressionStatement() override {
        delete expr;
    }
private:
    ExprAST* expr;
};

class ReturnStatement : public Statement {
public:
    explicit ReturnStatement(ExprAST* expr) : _expr(expr) {};

    void codegen() override;
private:
    ExprAST* _expr;
};

class EmptyStatement : public Statement {
    void codegen() override {}
};

class AssignStatement : public Statement {
    AssignStatement(std::string id, ExprAST* expr) : _id(std::move(id)), _expr(expr) {};
    void codegen() override;

    ~AssignStatement() override {
        delete _expr;
    }
private:
    std::string _id;
    ExprAST* _expr;
};

#endif //KOTLIN_LLVM_STATEMENT_HPP
