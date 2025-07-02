
#include "symbol_table.h"

bool SymbolTable::declare(const std::string& name, SymbolType type) {
    if (exists(name)) {
        return false; // Já declarada
    }
    symbols[name] = Symbol(type);
    return true;
}

bool SymbolTable::exists(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

bool SymbolTable::assign(const std::string& name, const std::variant<int, bool>& value) {
    if (!exists(name)) {
        return false;
    }
    symbols[name].value = value;
    symbols[name].initialized = true;
    return true;
}

Symbol* SymbolTable::get(const std::string& name) {
    auto it = symbols.find(name);
    return (it != symbols.end()) ? &it->second : nullptr;
}

void SymbolTable::clear() {
    symbols.clear();
}
