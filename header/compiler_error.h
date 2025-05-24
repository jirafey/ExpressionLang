#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

#include <string>
#include <iostream>
#include <cstdlib>

extern int yylineno;  // Declare yylineno as external

enum class CompilerError {
    None,
    SyntaxError,
    SemanticError,
    TypeMismatchOp,
    TypeMismatchAssign,
    VariableNotDeclared,
    NullOperandInternal,
    NullExpressionInternal,
    NullPointerInternal,
    UnsupportedTypeInternal,
    InternalLogicError,
    OutOfRegistersInt,
    OutOfRegistersFloat,
    AllocationFailed,
    IfConditionNotBool,
    MissingExcl,
    InvalidChar,
    DivisionByZero,
    RuntimeError
};

inline void throwCompilerError(CompilerError type, const std::string& message) {
    std::string error_message;
    switch (type) {
        case CompilerError::SyntaxError:
            error_message = "Error on line " + std::to_string(yylineno) + ": " + message;
            break;
        case CompilerError::SemanticError:
            error_message = "Error: " + message;
            break;
        case CompilerError::TypeMismatchOp:
            error_message = "Error: Type mismatch in operation: " + message;
            break;
        case CompilerError::TypeMismatchAssign:
            error_message = "Error: Type mismatch in assignment: " + message;
            break;
        case CompilerError::VariableNotDeclared:
            error_message = "Error: Variable not declared: " + message;
            break;
        case CompilerError::NullOperandInternal:
            error_message = "Error: Internal error - null operand: " + message;
            break;
        case CompilerError::NullExpressionInternal:
            error_message = "Error: Internal error - null expression: " + message;
            break;
        case CompilerError::InvalidChar:
            error_message = "Error on line " + std::to_string(yylineno) + ": " + message;
            break;
        case CompilerError::DivisionByZero:
            error_message = "Error: Division by zero: " + message;
            break;
        case CompilerError::RuntimeError:
            error_message = "Runtime error: " + message;
            break;
        default:
            error_message = "Error: Unknown error: " + message;
    }
    std::cerr << error_message << std::endl;
    exit(1);
}

#endif // COMPILER_ERROR_H
