#include "header/code_gen_helpers.h"
#include "header/code_gen_globals.h"
#include "header/compiler_error.h"
#include "header/expression.h"

std::string new_temporary_variable_label(TypeEnum type_arg) {
    std::string name = "_tmp" + std::to_string(temp_var_count++);
    if (type_arg == TypeEnum::Integer) {
        data_segment.push_back(name + ": .word 0");
    } else if (type_arg == TypeEnum::Float) {
        data_segment.push_back(name + ": .float 0.0");
    } else {
        throwCompilerError(CompilerError::UnsupportedTypeInternal,
            "Temp var for type " + std::to_string(static_cast<int>(type_arg)));
    }
    return name;
}

std::string new_float_literal_label(float val) {
    std::string l = "_flit" + std::to_string(float_lit_count++);
    data_segment.push_back(l + ": .float " + std::to_string(val));
    return l;
}

std::string new_control_label() {
    return "LBL" + std::to_string(label_count++);
}

std::string new_temp_label() {
    return "_temp" + std::to_string(temp_var_count++);
}

void emit(const std::string& i) {
    text_segment.push_back("\t" + i);
}

void emit_data(const std::string& d) {
    data_segment.push_back(d);
} 