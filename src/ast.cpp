#include "ast.hpp"

llvm::Value *ConstExprAST::codegen() {
    return nullptr;
}

llvm::Value *VarExprAST::codegen() {
    return nullptr;
}

BinaryExprAST::~BinaryExprAST() {
    delete first;
    delete second;
}

llvm::Value *AddExprAST::codegen() {
    return nullptr;
}

llvm::Value *SubExprAST::codegen() {
    return nullptr;
}

llvm::Value *MulExprAST::codegen() {
    return nullptr;
}

llvm::Value *DivExprAST::codegen() {
    return nullptr;
}
