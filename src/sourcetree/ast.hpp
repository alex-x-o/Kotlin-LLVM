#ifndef KOTLIN_LLVM_AST_HPP
#define KOTLIN_LLVM_AST_HPP

#include <utility>
#include <vector>
#include <string>

#include "llvm/IR/Value.h"

enum Type {
    INT, DOUBLE, STRING
};

llvm::Type* type_to_llvm_type(Type type);

class Param {
public:
    Param(std::string id, Type type) : _id(std::move(id)), _type(type) {};

    const std::string &getId() const {
        return _id;
    }

    Type getType() const {
        return _type;
    }

private:
    std::string _id;
    Type _type;
};

class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual llvm::Value* codegen() = 0;
};

class ConstExprAST : public ExprAST {
public:
    llvm::Value* codegen() override;
    explicit ConstExprAST(double value) : _value(value) {}
private:
    double _value;
};

class ConstStringExprAST : public ExprAST {
public:
    llvm::Value* codegen() override;
    explicit ConstStringExprAST(std::string value) : _value(std::move(value)) {}
private:
    std::string _value;
};

class VarExprAST : public ExprAST {
public:
    llvm::Value* codegen() override;
    explicit VarExprAST(std::string id) : _id(std::move(id)) {}
private:
    std::string _id;
};

class BinaryExprAST : public ExprAST {
public:
    BinaryExprAST(ExprAST* first, ExprAST* second) : first(first), second(second) {};
    ~BinaryExprAST() override;
protected:
    ExprAST *first, *second;
};

class AddExprAST : public BinaryExprAST {
public:
    AddExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class SubExprAST : public BinaryExprAST {
public:
    SubExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class MulExprAST : public BinaryExprAST {
public:
    MulExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class DivExprAST : public BinaryExprAST {
public:
    DivExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class CallExprAST : public ExprAST {
public:
    explicit CallExprAST(std::string callee_id, std::vector<ExprAST *> args) : _callee_id(std::move(callee_id)),
                                                                               _args(std::move(args)) {};
    llvm::Value* codegen() override;
private:
    std::string _callee_id;
    std::vector<ExprAST*> _args;
};

class ReturnAST : public ExprAST {
public:
    explicit ReturnAST(ExprAST* expr) : _expr(expr) {};

    llvm::Value* codegen() override;
private:
    ExprAST* _expr;
};

#endif //KOTLIN_LLVM_AST_HPP
