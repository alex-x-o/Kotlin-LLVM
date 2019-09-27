#include <iostream>

#include "ast.hpp"
#include "statement.hpp"
#include "parser.tab.hpp"

#include "llvm/IR/Value.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

extern llvm::LLVMContext context;
extern std::map<std::string, llvm::AllocaInst*> named_values;
extern llvm::IRBuilder<> builder;
extern llvm::Module* module;

extern void yyerror(std::string msg);

llvm::Type* type_to_llvm_type(Type type) {
    switch (type) {
        case INT:
            return llvm::Type::getInt32Ty(context);
        case DOUBLE:
            return llvm::Type::getDoubleTy(context);
        case STRING:
            return llvm::Type::getInt8PtrTy(context);
    }
}

llvm::Value *IntExprAST::codegen() {
    return llvm::ConstantInt::get(context, llvm::APInt(32, _value));
}

llvm::Value *DoubleExprAST::codegen() {
    return llvm::ConstantFP::get(context, llvm::APFloat(_value));
}

llvm::Value* ConstStringExprAST::codegen() {
    return builder.CreateGlobalStringPtr(llvm::StringRef(_value));
}

llvm::Value *ConstBooleanExprAST::codegen() {
    return llvm::ConstantInt::get(context, llvm::APInt(1, _value ? 1 : 0));
}

llvm::Value *VarExprAST::codegen() {
    llvm::Value* value = named_values[_id];
    if (value == nullptr) {
        std::cerr << "Unknown variable name: " << _id << std::endl;
        exit(EXIT_FAILURE);
    }
    return builder.CreateLoad(value, _id);
}

UnaryExprAST::~UnaryExprAST() {
    delete _first;
}

llvm::Value *InvExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    if (value_first == nullptr) {
        yyerror("Error");
    }
    return builder.CreateNot(value_first, "invtmp");
}

llvm::Value *NotLExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    if (value_first == nullptr) {
        yyerror("Error");
    }
    return llvm::ConstantInt::get(context, llvm::APInt(1, value_first ? 0 : 1));
}

BinaryExprAST::~BinaryExprAST() {
    delete _first;
    delete _second;
}

llvm::Value *AddExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    else if(value_first->getType() == llvm::Type::getInt32Ty(context) &&
    value_second->getType() == llvm::Type::getInt32Ty(context))
        return builder.CreateAdd(value_first, value_second, "addtmp");
    else
        return builder.CreateFAdd(value_first, value_second, "addtmp");
}

llvm::Value *SubExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    else if(value_first->getType() == llvm::Type::getInt32Ty(context) &&
            value_second->getType() == llvm::Type::getInt32Ty(context))
        return builder.CreateSub(value_first, value_second, "subtmp");
    else
        return builder.CreateFSub(value_first, value_second, "subtmp");
}

llvm::Value *MulExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    else if(value_first->getType() == llvm::Type::getInt32Ty(context) &&
            value_second->getType() == llvm::Type::getInt32Ty(context))
        return builder.CreateMul(value_first, value_second, "multmp");
    else
        return builder.CreateFMul(value_first, value_second, "subtmp");
}

llvm::Value *DivExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    else if(value_first->getType() == llvm::Type::getInt32Ty(context) &&
            value_second->getType() == llvm::Type::getInt32Ty(context))
        return builder.CreateSDiv(value_first, value_second, "divtmp");
    else
        return builder.CreateFAdd(value_first, value_second, "divtmp");
}

llvm::Value *ModExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    else if(value_first->getType() == llvm::Type::getInt32Ty(context) &&
            value_second->getType() == llvm::Type::getInt32Ty(context))
        return builder.CreateSRem(value_first, value_second, "modtmp");
    else
        return builder.CreateFRem(value_first, value_second, "modtmp");
}

llvm::Value *LessExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateICmpSLT(value_first, value_second, "lesstmp");
}

llvm::Value *GrtExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateICmpSGT(value_first, value_second, "grttmp");
}

llvm::Value *LEExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateICmpSLE(value_first, value_second, "leetmp");
}

llvm::Value *GEExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateICmpSGE(value_first, value_second, "geetmp");
}

llvm::Value *AndExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateAnd(value_first, value_second, "andtmp");
}

llvm::Value *OrExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateOr(value_first, value_second, "ortmp");
}

llvm::Value *XorExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateXor(value_first, value_second, "xortmp");
}

llvm::Value *ShlExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateShl(value_first, value_second, "shltmp");
}

llvm::Value *ShrExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateLShr(value_first, value_second, "shrtmp");
}

llvm::Value *AndLExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    else if (value_first->getType() != llvm::Type::getInt1Ty(context)
        || value_second->getType() != llvm::Type::getInt1Ty(context)) {
            yyerror("Must be boolean");
    }
    // TODO Fix
    llvm::Value *lhs = llvm::ConstantInt::get(context, llvm::APInt(1, value_first ? 1 : 0));
    llvm::Value *rhs = llvm::ConstantInt::get(context, llvm::APInt(1, value_second ? 1 : 0));
    if(lhs && rhs)
        return llvm::ConstantInt::get(context, llvm::APInt(1, 1));
    else
        return llvm::ConstantInt::get(context, llvm::APInt(1, 0));
}

llvm::Value *OrLExprAST::codegen() {
    llvm::Value *value_first = _first->codegen();
    llvm::Value *value_second = _second->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    else if (value_first->getType() != llvm::Type::getInt1Ty(context)
        || value_second->getType() != llvm::Type::getInt1Ty(context)) {
        yyerror("Must be boolean");
    }
    // TODO Fix
    llvm::Value *lhs = builder.CreateICmpEQ(value_first, llvm::ConstantInt::get(context, llvm::APInt(8, 1)), "ifcondl");
    llvm::Value *rhs = builder.CreateICmpEQ(value_first, llvm::ConstantInt::get(context, llvm::APInt(8, 1)), "ifcondl");
    if(lhs && rhs)
        return llvm::ConstantInt::get(context, llvm::APInt(1, 0));
    else
        return llvm::ConstantInt::get(context, llvm::APInt(1, 1));
}

llvm::Value *CallExprAST::codegen() {
    llvm::Function *callee_function = module->getFunction(_callee_id);
    if (callee_function == nullptr) {
        yyerror("Function " + _callee_id + " doesn't exist");
    }

    unsigned arg_size = callee_function->arg_size();

    if (arg_size != _args.size()) {
        yyerror("Wrong number of arguments: " + _callee_id);
    }

    std::vector<llvm::Value*> generated_args;
    for (unsigned i = 0; i < arg_size; ++i) {
        llvm::Value* arg_value = _args[i]->codegen();
        if (arg_value == nullptr) {
            return nullptr;
        }
        generated_args.push_back(arg_value);
    }

    return builder.CreateCall(callee_function, generated_args, "calltmp");
}

void ReturnStatement::codegen() {
    llvm::Value* expression_value = _expr->codegen();
    builder.CreateRet(expression_value);
}

llvm::Value *IfElseExprAST::codegen() {
    llvm::Value *cond_value = _cond->codegen();

    if (cond_value->getType() != llvm::Type::getInt1Ty(context)) {
        yyerror("The condition of the if expression must be boolean");
    }

    llvm::Function *function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(context, "iftrue", function);
    llvm::BasicBlock *else_block = llvm::BasicBlock::Create(context, "iffalse");
    llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(context, "ifcont");

    builder.CreateCondBr(cond_value, then_block, else_block);

    builder.SetInsertPoint(then_block);

    llvm::Value *then_value = _then_expr->codegen();
    if(then_value == nullptr)
        return nullptr;

    builder.CreateBr(merge_block);

    then_block = builder.GetInsertBlock();

    function->getBasicBlockList().push_back(else_block);
    builder.SetInsertPoint(else_block);

    llvm::Value *else_value = _else_expr->codegen();
    if(else_value == nullptr)
        return nullptr;

    builder.CreateBr(merge_block);

    if (then_value->getType() != else_value->getType()) {
        yyerror("If branches must have the same value");
    }

    function->getBasicBlockList().push_back(merge_block);
    builder.SetInsertPoint(merge_block);
    llvm::PHINode* phi_node = builder.CreatePHI(then_value->getType(), 2, "iftmp");

    phi_node->addIncoming(then_value, then_block);
    phi_node->addIncoming(else_value, else_block);
    return phi_node;
}