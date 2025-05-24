#ifndef CODE_GEN_HELPERS_H
#define CODE_GEN_HELPERS_H

#include <string>
#include "expression.h"

// string manipulation
char* safe_strdup(const char* s, const std::string& context);

// Type promotion
void handle_type_promotion(ExpressionNode*& op1, ExpressionNode*& op2, TypeEnum& type1, TypeEnum& type2);

// Assignment operations
ExpressionNode* przyp(const std::string& target_name, ExpressionNode* val_expr);

// Comparison operations
ExpressionNode* decl(ExpressionNode* op1, char* comp_op_chars, ExpressionNode* op2);

// Binary operations
ExpressionNode* process_binary_op(ExpressionNode* op1, ExpressionNode* op2,
    const std::string& op_str);

// I/O operations
ExpressionNode* read(const std::string& var_name);
ExpressionNode* print(ExpressionNode* expr_to_print);

// Register management
std::string new_temp_int_register();
void free_temp_int_register();
std::string new_temp_float_register();
void free_temp_float_register();
void free_expr_reg(TypeEnum type_arg);
std::string load_expr_to_reg(ExpressionNode* expr_arg);

// Label management
std::string new_temporary_variable_label(TypeEnum type_arg);
std::string new_float_literal_label(float val);
std::string new_control_label();
std::string new_temp_label();
std::string new_float_literal(float value);

// MIPS code emission
void emit(const std::string& instruction);
void emit_data(const std::string& declaration);

// MIPS output
void output_mips_code();

#endif //CODE_GEN_HELPERS_H
