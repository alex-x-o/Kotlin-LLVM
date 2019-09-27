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

class IntExprAST : public ExprAST {
public:
    llvm::Value* codegen() override;
    explicit IntExprAST(int value) : _value(value) {}
private:
    int _value;
};

class DoubleExprAST : public ExprAST {
public:
    llvm::Value* codegen() override;
    explicit DoubleExprAST(double value) : _value(value) {}
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

class ConstBooleanExprAST : public ExprAST {
public:
    llvm::Value* codegen() override;
    explicit ConstBooleanExprAST(bool value) : _value(value) {};
private:
    bool _value;
};

class VarExprAST : public ExprAST {
public:
    llvm::Value* codegen() override;
    explicit VarExprAST(std::string id) : _id(std::move(id)) {}
private:
    std::string _id;
};

class UnaryExprAST : public ExprAST {
public:
    UnaryExprAST(ExprAST* first)
            : _first(first) {};
    ~UnaryExprAST() override;
protected:
    ExprAST *_first;
};

class InvExprAST : public UnaryExprAST {
public:
    InvExprAST(ExprAST* first) : UnaryExprAST(first) {};
    llvm::Value* codegen() override;
};

class NotLExprAST : public UnaryExprAST {
public:
    NotLExprAST(ExprAST* first) : UnaryExprAST(first) {};
    llvm::Value* codegen() override;
};

class BinaryExprAST : public ExprAST {
public:
    BinaryExprAST(ExprAST* first, ExprAST* second)
    : _first(first), _second(second) {};
    ~BinaryExprAST() override;
protected:
    ExprAST *_first, *_second;
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

class ModExprAST : public BinaryExprAST {
public:
    ModExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class LessExprAST : public BinaryExprAST {
public:
    LessExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class GrtExprAST : public BinaryExprAST {
public:
    GrtExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class LEExprAST : public BinaryExprAST {
public:
    LEExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class GEExprAST : public BinaryExprAST {
public:
    GEExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class AndExprAST : public BinaryExprAST {
public:
    AndExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class OrExprAST : public BinaryExprAST {
public:
    OrExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class XorExprAST : public BinaryExprAST {
public:
    XorExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class ShlExprAST : public BinaryExprAST {
public:
    ShlExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class ShrExprAST : public BinaryExprAST {
public:
    ShrExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class AndLExprAST : public BinaryExprAST {
public:
    AndLExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
    llvm::Value* codegen() override;
};

class OrLExprAST : public BinaryExprAST {
public:
    OrLExprAST(ExprAST* first, ExprAST* second) : BinaryExprAST(first, second) {};
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

class IfElseExprAST : public ExprAST {
public:
    IfElseExprAST(ExprAST* cond, ExprAST* then_expr, ExprAST* else_expr)
    : _cond(cond), _then_expr(then_expr), _else_expr(else_expr) {};
    llvm::Value* codegen() override;

    ~IfElseExprAST() override {
        delete _cond;
        delete _then_expr;
        delete _else_expr;
    }

private:
    ExprAST* _cond;
    ExprAST* _then_expr;
    ExprAST* _else_expr;
};

#endif //KOTLIN_LLVM_AST_HPP
