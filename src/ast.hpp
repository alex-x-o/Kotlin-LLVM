#ifndef KOTLIN_LLVM_AST_HPP
#define KOTLIN_LLVM_AST_HPP

#include <utility>

#include "llvm/IR/Value.h"

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

#endif //KOTLIN_LLVM_AST_HPP
