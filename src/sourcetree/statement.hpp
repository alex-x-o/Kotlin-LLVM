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

class VarDeclarationStatement: public Statement {
public:
    VarDeclarationStatement(std::string id, Type type, bool mut = true) :
    _id(std::move(id)), _type(type), _mut(mut) {};
    void codegen() override;

    const std::string &getId() const {
        return _id;
    }

private:
    std::string _id;
    Type _type;
    bool _mut;
};

class AssignStatement : public Statement {
public:
    AssignStatement(std::string id, ExprAST* expr) : _id(std::move(id)), _expr(expr) {};
    void codegen() override;

    ~AssignStatement() override {
        delete _expr;
    }
private:
    std::string _id;
    ExprAST* _expr;
};

class PlusAssignStatement : public Statement {
public:
    PlusAssignStatement(std::string id, ExprAST* expr) : _id(std::move(id)), _expr(expr) {};
    void codegen() override;

    ~PlusAssignStatement() override {
        delete _expr;
    }
private:
    std::string _id;
    ExprAST* _expr;
};

class MinusAssignStatement : public Statement {
public:
    MinusAssignStatement(std::string id, ExprAST* expr) : _id(std::move(id)), _expr(expr) {};
    void codegen() override;

    ~MinusAssignStatement() override {
        delete _expr;
    }
private:
    std::string _id;
    ExprAST* _expr;
};

class TimesAssignStatement : public Statement {
public:
    TimesAssignStatement(std::string id, ExprAST* expr) : _id(std::move(id)), _expr(expr) {};
    void codegen() override;

    ~TimesAssignStatement() override {
        delete _expr;
    }
private:
    std::string _id;
    ExprAST* _expr;
};

class DivAssignStatement : public Statement {
public:
    DivAssignStatement(std::string id, ExprAST* expr) : _id(std::move(id)), _expr(expr) {};
    void codegen() override;

    ~DivAssignStatement() override {
        delete _expr;
    }
private:
    std::string _id;
    ExprAST* _expr;
};

class ModAssignStatement : public Statement {
public:
    ModAssignStatement(std::string id, ExprAST* expr) : _id(std::move(id)), _expr(expr) {};
    void codegen() override;

    ~ModAssignStatement() override {
        delete _expr;
    }
private:
    std::string _id;
    ExprAST* _expr;
};

class DeclareAndAssignStatement : public Statement {
public:
    DeclareAndAssignStatement(VarDeclarationStatement* decl_statement, AssignStatement* assign_statement)
    : _decl_statement(decl_statement), _assign_statement(assign_statement) {};

    void codegen() override;

    ~DeclareAndAssignStatement() override {
        delete _decl_statement;
        delete _assign_statement;
    }
private:
    VarDeclarationStatement* _decl_statement;
    AssignStatement* _assign_statement;
};

class IfStatement : public Statement {
public:
    IfStatement(ExprAST* cond, std::vector<Statement*>* then_stat)
            : _cond(cond), _then_stat(then_stat) {};
    void codegen() override;

    ~IfStatement() override {
        delete _cond;
        for(auto &i : *_then_stat)
            delete i;
    }

private:
    ExprAST* _cond;
    std::vector<Statement*>* _then_stat;
};

class IfElseStatement : public Statement {
public:
    IfElseStatement(ExprAST* cond, std::vector<Statement*>* then_stat, std::vector<Statement*>*  else_stat)
            : _cond(cond), _then_stat(then_stat), _else_stat(else_stat) {};
    void codegen() override;

    ~IfElseStatement() override {
        delete _cond;
        for(auto &i : *_then_stat)
            delete i;
        for(auto &i : *_else_stat)
            delete i;
    }

private:
    ExprAST* _cond;
    std::vector<Statement*>* _then_stat;
    std::vector<Statement*>* _else_stat;
};

class PrintStatement : public Statement {
public:
    PrintStatement(ExprAST* e)
    : _e(e) {}
    void codegen() override;

    ~PrintStatement() override {
        delete _e;
    }

private:
    ExprAST* _e;
};

class WhileStatement : public Statement {
public:
    WhileStatement(ExprAST* cond, std::vector<Statement*>* then_stat)
            : _cond(cond), _then_stat(then_stat) {};
    void codegen() override;

    ~WhileStatement() override {
        delete _cond;
        for(auto &i : *_then_stat)
            delete i;
    }

private:
    ExprAST* _cond;
    std::vector<Statement*>* _then_stat;
};

class ForStatement : public Statement {
public:
    ForStatement(std::string id, int start, int end, ExprAST* inc, std::vector<Statement*>* block)
    :_id(id), _start(start), _end(end), _inc(inc), _block(block) {};
    void codegen() override;

    ~ForStatement() override {
        for(auto &i : *_block)
            delete i;
    }
private:
    std::string _id;
    int _start;
    int _end;
    ExprAST* _inc;
    std::vector<Statement*>* _block;
};

class ForUStatement : public Statement {
public:
    ForUStatement(std::string id, int start, int end, ExprAST* inc, std::vector<Statement*>* block)
            :_id(id), _start(start), _end(end), _inc(inc), _block(block) {};
    void codegen() override;

    ~ForUStatement() override {
        for(auto &i : *_block)
            delete i;
    }
private:
    std::string _id;
    int _start;
    int _end;
    ExprAST* _inc;
    std::vector<Statement*>* _block;
};

#endif //KOTLIN_LLVM_STATEMENT_HPP
