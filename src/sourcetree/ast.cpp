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

llvm::Value *ConstExprAST::codegen() {
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

BinaryExprAST::~BinaryExprAST() {
    delete first;
    delete second;
}

llvm::Value *AddExprAST::codegen() {
    llvm::Value *value_first = first->codegen();
    llvm::Value *value_second = first->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateFAdd(value_first, value_second, "addtmp");
}

llvm::Value *SubExprAST::codegen() {
    llvm::Value *value_first = first->codegen();
    llvm::Value *value_second = first->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateFSub(value_first, value_second, "subtmp");
}

llvm::Value *MulExprAST::codegen() {
    llvm::Value *value_first = first->codegen();
    llvm::Value *value_second = first->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateFMul(value_first, value_second, "multmp");
}

llvm::Value *DivExprAST::codegen() {
    llvm::Value *value_first = first->codegen();
    llvm::Value *value_second = first->codegen();
    if (value_first == nullptr || value_second == nullptr) {
        yyerror("Error");
    }
    return builder.CreateFDiv(value_first, value_second, "divtmp");
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

llvm::Value *IfExprAST::codegen() {
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

    builder.CreateBr(merge_block);

    then_block = builder.GetInsertBlock();

    function->getBasicBlockList().push_back(else_block);
    builder.SetInsertPoint(else_block);

    llvm::Value *else_value = _else_expr->codegen();

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