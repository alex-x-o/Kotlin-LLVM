#include "allocation.hpp"

llvm::AllocaInst *create_entry_block_alloca(llvm::Function *function, const std::string &var_name, llvm::Type* type) {
    llvm::IRBuilder<> tmp_builder(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmp_builder.CreateAlloca(type, nullptr, var_name);
}
