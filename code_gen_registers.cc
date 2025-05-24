#include "header/code_gen_helpers.h"
#include "header/code_gen_globals.h"
#include "header/compiler_error.h"
#include "header/expression.h"

std::string new_temp_int_register() {
    if (temp_int_reg_count > 7) {
        throwCompilerError(CompilerError::OutOfRegistersInt, "Out of integer registers (max 8)");
    }
    return "$t" + std::to_string(temp_int_reg_count++);
}

void free_temp_int_register() {
    if (temp_int_reg_count > 0) {
        temp_int_reg_count--;
    }
}

std::string new_temp_float_register() {
    if (temp_float_reg_count > 30) {
        throwCompilerError(CompilerError::OutOfRegistersFloat, "Out of float registers (max 16)");
    }
    std::string reg = "$f" + std::to_string(temp_float_reg_count);
    temp_float_reg_count += 2;
    return reg;
}

void free_temp_float_register() {
    if (temp_float_reg_count > 0) {
        temp_float_reg_count -= 2;
    }
}

void free_expr_reg(TypeEnum t) {
    if (t == TypeEnum::Integer) {
        free_temp_int_register();
    } else if (t == TypeEnum::Float) {
        free_temp_float_register();
    }
}

std::string load_expr_to_reg(ExpressionNode* expr) {
    if (!expr) {
        throwCompilerError(CompilerError::NullExpressionInternal, "load_expr_to_reg called with null expression");
    }

    if (expr->type == TypeEnum::Integer) {
        std::string r = new_temp_int_register();
        if (expr->is_literal) {
            emit("li " + r + ", " + std::to_string(expr->ival));
        } else {
            if (expr->name.empty()) {
                throwCompilerError(CompilerError::InternalLogicError, "Integer variable has no name");
            }
            emit("lw " + r + ", " + expr->name);
        }
        return r;
    } else if (expr->type == TypeEnum::Float) {
        std::string fr = new_temp_float_register();
        std::string temp_reg = new_temp_int_register();
        if (expr->is_literal) {
            std::string flit_label = new_float_literal_label(expr->fval);
            emit("la " + temp_reg + ", " + flit_label);
            emit("l.s " + fr + ", (" + temp_reg + ")");
        } else {
            if (expr->name.empty()) {
                throwCompilerError(CompilerError::InternalLogicError, "Float variable has no name");
            }
            emit("la " + temp_reg + ", " + expr->name);
            emit("l.s " + fr + ", (" + temp_reg + ")");
        }
        free_temp_int_register();
        return fr;
    } else {
        throwCompilerError(CompilerError::UnsupportedTypeInternal, 
            "load_expr_to_reg called with unsupported type: " + std::to_string(static_cast<int>(expr->type)));
        return "";
    }
} 