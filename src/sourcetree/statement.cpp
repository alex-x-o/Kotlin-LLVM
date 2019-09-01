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

void FunctionAST::codegen() {
    llvm::Function *function = module->getFunction(_prototype->getId());

    if (function == nullptr) {
        function = _prototype->codegen();
    }

    if (function == nullptr) {
        yyerror("Error generating function");
        return;
    }

    if (!function->empty()) {
        yyerror("Cannot redefine function: " + _prototype->getId());
    }

    llvm::BasicBlock* basic_block = llvm::BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(basic_block);

    named_values.clear();
    for (auto &arg : function->args()) {
        named_values[arg.getName()] = &arg;
    }

    _body->codegen();

    llvm::verifyFunction(*function);
}

FunctionAST::~FunctionAST() {
    delete _prototype;
    delete _body;
}

llvm::Function* FunctionPrototypeAST::codegen() {
    std::vector<llvm::Type *> param_types;

    for (Param *param : _params) {
        llvm::Type *type = type_to_llvm_type(param->getType());
        param_types.push_back(type);
    }

    llvm::Type *return_type = type_to_llvm_type(_return_type);

    llvm::FunctionType *function_type = llvm::FunctionType::get(return_type, param_types, false);

    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, _id, module);

    unsigned i = 0;
    for (auto &param : function->args()) {
        param.setName(_params[i++]->getId());
    }
    return function;
}

void ExternalFunctionStatement::codegen() {
    _prototype->codegen();
}

ExternalFunctionStatement::~ExternalFunctionStatement() {
    delete _prototype;
}