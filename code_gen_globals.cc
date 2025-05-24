#include "header/code_gen_globals.h"
#include "header/code_gen_helpers.h"
#include "header/TypeEnum.h"
#include "header/compiler_error.h"
#include <iostream>

std::vector<std::string> text_segment;
std::vector<std::string> data_segment;
int temp_int_reg_count = 0;
int temp_float_reg_count = 0;
int temp_var_count = 0;
int float_lit_count = 0;
int label_count = 0;
bool float_zero_defined = false;
std::string float_zero_label = "_float_zero";

// Global variables
std::map<std::string, Symbol> sym_table;

// Basic register management
std::string new_temp_register() {
    if (temp_int_reg_count >= 10) {
        throwCompilerError(CompilerError::OutOfRegistersInt, "Out of temporary registers");
    }
    return "$t" + std::to_string(temp_int_reg_count++);
}

void free_temp_register() {
    if (temp_int_reg_count > 0) {
        temp_int_reg_count--;
    }
}