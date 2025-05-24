#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include "TypeEnum.h"

enum class Operation {
    Add,
    Sub,
    Mul,
    Div
};

inline std::string typeToString(TypeEnum type) {
    switch (type) {
        case TypeEnum::Integer: return "int";
        case TypeEnum::Float: return "float";
        default: return "unknown";
    }
}

struct ExpressionNode {
    TypeEnum type;
    bool is_literal;
    int ival;
    float fval;
    std::string name;
    Operation op;

    ExpressionNode() : type(TypeEnum::Integer), is_literal(false), ival(0), fval(0.0f), 
                      name(""), op(Operation::Add) {}

    ExpressionNode(TypeEnum t, const std::string& n) 
        : type(t), is_literal(false), ival(0), fval(0.0f), name(n), 
          op(Operation::Add) {}

    ExpressionNode(TypeEnum t, int val) 
        : type(t), is_literal(true), ival(val), fval(0.0f), name(""), 
          op(Operation::Add) {}

    ExpressionNode(TypeEnum t, float val) 
        : type(t), is_literal(true), ival(0), fval(val), name(""), 
          op(Operation::Add) {}
};

#endif // EXPRESSION_H
