#ifndef CODE_GEN_LABELS_H
#define CODE_GEN_LABELS_H

#include <string>
#include "TypeEnum.h"

// Label management functions
std::string new_temp_label();
std::string new_temporary_variable_label(TypeEnum type);
std::string new_float_literal_label(float val);
std::string new_control_label();

// MIPS code emission
void emit(const std::string& instruction);
void emit_data(const std::string& declaration);

#endif // CODE_GEN_LABELS_H 