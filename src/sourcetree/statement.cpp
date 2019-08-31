#include "statement.hpp"

#include "llvm/IR/Value.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

extern llvm::LLVMContext context;
extern std::map<std::string, llvm::Value*> named_values;
extern llvm::IRBuilder<> builder;
extern llvm::Module* module;

extern void yyerror(std::string msg);

// TODO migrate implementation to reuse FunctionPrototype
llvm::Function* FunctionAST::codegen() {
    std::vector<llvm::Type *> param_types;

    for (Param* param : _params) {
        llvm::Type *type = type_to_llvm_type(param->getType());
        param_types.push_back(type);
    }

    llvm::Type *return_type = type_to_llvm_type(_return_type);

    llvm::FunctionType* function_type = llvm::FunctionType::get(return_type, param_types, false);

    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, _id, module);

    unsigned i = 0;
    for (auto &param : function->args()) {
        param.setName(_params[i++]->getId());
    }

    // TODO this is not working, functions can still be declared
    if (!function->empty()) {
        yyerror("Function already declared: " + _id);
    }

    llvm::BasicBlock* basic_block = llvm::BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(basic_block);

    named_values.clear();
    for (auto &arg : function->args()) {
        named_values[arg.getName()] = &arg;
    }

    _body->codegen();

    llvm::verifyFunction(*function);

    return function;
}

llvm::Function* FunctionPrototypeAST::codegen() {
    std::vector<llvm::Type *> param_types;

    for (Param* param : _params) {
        llvm::Type *type = type_to_llvm_type(param->getType());
        param_types.push_back(type);
    }

    llvm::Type *return_type = type_to_llvm_type(_return_type);

    llvm::FunctionType* function_type = llvm::FunctionType::get(return_type, param_types, false);

    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, _id, module);

    unsigned i = 0;
    for (auto &param : function->args()) {
        param.setName(_params[i++]->getId());
    }
    return function;
}

#include "statement.hpp"
