#include <iostream>

#include "ast.hpp"
#include "statement.hpp"
#include "parser.tab.hpp"

#include "llvm/IR/Value.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

extern llvm::LLVMContext context;
extern std::map<std::string, llvm::Value*> named_values;
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

llvm::Value *VarExprAST::codegen() {
    llvm::Value* value = named_values[_id];
    if (value == nullptr) {
        std::cerr << "Unknown variable name: " << _id << std::endl;
        exit(EXIT_FAILURE);
    }
    return value;
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
