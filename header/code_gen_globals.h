#ifndef CODE_GEN_GLOBALS_H
#define CODE_GEN_GLOBALS_H

#include <string>
#include <vector>
#include <map>
#include "expression.h"
#include "TypeEnum.h"

struct Symbol {
    std::string name;
    TypeEnum type;
};

// extern because in .cc
extern std::map<std::string, Symbol> sym_table; // declared values
extern std::vector<std::string> data_segment; // .data
extern std::vector<std::string> text_segment; // .text

extern int temp_int_reg_count; // num of int registers used
extern int temp_float_reg_count;
extern int temp_var_count;
extern int float_lit_count; // how many float literals that are not variables are used
extern int label_count; // control flow labels
// (like LBL0, LBL1 for if statements or division checks)

extern bool runtime_error_handler_needed;
extern std::string runtime_error_msg_label;
extern std::string runtime_error_handler_label;
extern std::string float_zero_label; // for mips .data
extern bool float_zero_defined; // bool for the same purpose

// Global variables for code generation
extern std::map<std::string, TypeEnum> symbol_table;

// Basic register management
std::string new_temp_register();
void free_temp_register();

// Runtime error handling
void emit_runtime_error_handler();
void emit_runtime_error_messages();

#endif // CODE_GEN_GLOBALS_H
