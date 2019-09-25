#include "statement.hpp"

#include "llvm/IR/Value.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "allocation.hpp"

extern llvm::LLVMContext context;
extern std::map<std::string, llvm::AllocaInst*> named_values;
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
        llvm::AllocaInst* alloca = create_entry_block_alloca(function, arg.getName(), arg.getType());

        builder.CreateStore(&arg, alloca);

        named_values[arg.getName()] = alloca;
    }

    for (Statement* statement : *_body) {
        statement->codegen();
    }

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

void AssignStatement::codegen() {
    llvm::Value* lhs = named_values[_id];
    if (lhs == nullptr) {
        yyerror("Unknown variable: " + _id);
    }
    llvm::Value* rhs = _expr->codegen();

    builder.CreateStore(rhs, lhs);
}


void VarDeclarationStatement::codegen() {
    llvm::Type* llvm_type = type_to_llvm_type(_type);
    llvm::AllocaInst* alloca = builder.CreateAlloca(llvm_type, nullptr, _id);
    named_values[_id] = alloca;

}

void DeclareAndAssignStatement::codegen() {
    _decl_statement->codegen();
    _assign_statement->codegen();
}

void IfElseStatement::codegen() {
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

    for(auto &i: *_then_stat)
        i->codegen();

    builder.CreateBr(merge_block);

    then_block = builder.GetInsertBlock();

    function->getBasicBlockList().push_back(else_block);
    builder.SetInsertPoint(else_block);

    for(auto &i: *_else_stat)
        i->codegen();

    builder.CreateBr(merge_block);

    function->getBasicBlockList().push_back(merge_block);
    builder.SetInsertPoint(merge_block);
}
