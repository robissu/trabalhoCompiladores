
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <unordered_map>
#include <string>
#include <variant>

enum class SymbolType {
    INTEIRO,
    LOGICO
};

struct Symbol {
    SymbolType type;
    std::variant<int, bool> value;
    bool initialized;
    
    Symbol(SymbolType t = SymbolType::INTEIRO) 
        : type(t), value(0), initialized(false) {}
};

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> symbols;
    
public:
    bool declare(const std::string& name, SymbolType type);
    bool exists(const std::string& name) const;
    bool assign(const std::string& name, const std::variant<int, bool>& value);
    Symbol* get(const std::string& name);
    void clear();
};

#endif
