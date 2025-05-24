#include "header/code_gen_helpers.h"
#include "header/code_gen_globals.h"
#include "header/compiler_error.h"
#include "header/expression.h"
#include <string>

// New function to handle type checking and promotion
void handle_type_promotion(ExpressionNode*& op1, ExpressionNode*& op2, TypeEnum& type1, TypeEnum& type2) {
    if (type1 == TypeEnum::Integer && type2 == TypeEnum::Float) {
        std::string op1_int_reg = load_expr_to_reg(op1);
        std::string promoted_freg = new_temp_float_register();
        emit("mtc1 " + op1_int_reg + ", " + promoted_freg);
        emit("cvt.s.w " + promoted_freg + ", " + promoted_freg);
        free_expr_reg(TypeEnum::Integer);
        std::string temp_promoted_label = new_temporary_variable_label(TypeEnum::Float);
        emit("s.s " + promoted_freg + ", " + temp_promoted_label);
        free_temp_float_register();
        delete op1;
        op1 = new ExpressionNode();
        op1->type = TypeEnum::Float;
        op1->is_literal = false;
        op1->name = temp_promoted_label;
        type1 = TypeEnum::Float;
    } else if (type1 == TypeEnum::Float && type2 == TypeEnum::Integer) {
        std::string op2_int_reg = load_expr_to_reg(op2);
        std::string promoted_freg = new_temp_float_register();
        emit("mtc1 " + op2_int_reg + ", " + promoted_freg);
        emit("cvt.s.w " + promoted_freg + ", " + promoted_freg);
        free_expr_reg(TypeEnum::Integer);
        std::string temp_promoted_label = new_temporary_variable_label(TypeEnum::Float);
        emit("s.s " + promoted_freg + ", " + temp_promoted_label);
        free_temp_float_register();
        delete op2;
        op2 = new ExpressionNode();
        op2->type = TypeEnum::Float;
        op2->is_literal = false;
        op2->name = temp_promoted_label;
        type2 = TypeEnum::Float;
    }
}

ExpressionNode* read(const std::string& var_name) {
    if (sym_table.find(var_name) == sym_table.end()) {
        throwCompilerError(CompilerError::VariableNotDeclared, "read target: " + var_name);
    }
    TypeEnum target_type = sym_table[var_name].type;
    if (target_type == TypeEnum::Integer) {
        emit("li $v0, 5");
        emit("syscall");
        emit("sw $v0, " + var_name);
    } else if (target_type == TypeEnum::Float) {
        emit("li $v0, 6");
        emit("syscall");
        emit("s.s $f0, " + var_name);
    } else {
        throwCompilerError(CompilerError::UnsupportedTypeInternal, "Cannot read into variable '" + var_name + "' of unsupported type.");
    }
    return nullptr;  // read doesn't return a value
}

ExpressionNode* print(ExpressionNode* expr_to_print) {
    if (!expr_to_print) {
        throwCompilerError(CompilerError::NullExpressionInternal, "print_stmt received null expression");
    }
    std::string val_reg;
    try {
        val_reg = load_expr_to_reg(expr_to_print);
        if (expr_to_print->type == TypeEnum::Integer) {
            emit("li $v0, 1");
            emit("move $a0, " + val_reg);
            emit("syscall");
            free_expr_reg(TypeEnum::Integer);
        } else if (expr_to_print->type == TypeEnum::Float) {
            emit("li $v0, 2");
            emit("mov.s $f12, " + val_reg);
            emit("syscall");
            free_expr_reg(TypeEnum::Float);
        }
        emit("li $v0, 4");
        emit("la $a0, _newline_char");
        emit("syscall");
    } catch (...) {
        delete expr_to_print;
        throw;
    }
    return nullptr;  // print doesn't return a value
}

ExpressionNode* decl(ExpressionNode* op1, char* comp_op_chars, ExpressionNode* op2) {
    if (!op1 || !op2) {
        if(op1) delete op1;
        if(op2) delete op2;
        if(comp_op_chars) free(comp_op_chars);
        throwCompilerError(CompilerError::NullOperandInternal, "Comparison operation");
    }

    std::string comp_op_str(comp_op_chars);
    TypeEnum type1 = op1->type;
    TypeEnum type2 = op2->type;

    // Handle type promotion
    handle_type_promotion(op1, op2, type1, type2);

    if (type1 != type2) {
        throwCompilerError(CompilerError::InternalLogicError, "Type mismatch after promotion for '" + comp_op_str + "'");
    }

    std::string result_bool_reg = new_temp_int_register();
    std::string temp_bool_var_label;

    if (type1 == TypeEnum::Integer) {
        // Integer comparisons
        std::string r1_str = load_expr_to_reg(op1);
        std::string r2_str = load_expr_to_reg(op2);

        if (comp_op_str == "==") { emit("seq " + result_bool_reg + ", " + r1_str + ", " + r2_str); }
        else if (comp_op_str == "~=") { emit("sne " + result_bool_reg + ", " + r1_str + ", " + r2_str); }
        else if (comp_op_str == "<") { emit("slt " + result_bool_reg + ", " + r1_str + ", " + r2_str); }
        else if (comp_op_str == "<=") { emit("sle " + result_bool_reg + ", " + r1_str + ", " + r2_str); }
        else if (comp_op_str == ">") { emit("sgt " + result_bool_reg + ", " + r1_str + ", " + r2_str); }
        else if (comp_op_str == ">=") { emit("sge " + result_bool_reg + ", " + r1_str + ", " + r2_str); }

        free_expr_reg(TypeEnum::Integer);
        free_expr_reg(TypeEnum::Integer);
    } else {
        // Float comparisons
        std::string fr1_str = load_expr_to_reg(op1);
        std::string fr2_str = load_expr_to_reg(op2);

        if (comp_op_str == "==") { emit("c.eq.s " + fr1_str + ", " + fr2_str); }
        else if (comp_op_str == "~=") { emit("c.eq.s " + fr1_str + ", " + fr2_str); }
        else if (comp_op_str == "<") { emit("c.lt.s " + fr1_str + ", " + fr2_str); }
        else if (comp_op_str == "<=") { emit("c.le.s " + fr1_str + ", " + fr2_str); }
        else if (comp_op_str == ">") { emit("c.lt.s " + fr2_str + ", " + fr1_str); }
        else if (comp_op_str == ">=") { emit("c.le.s " + fr2_str + ", " + fr1_str); }

        emit("bc1t LBL" + std::to_string(label_count));
        emit("li " + result_bool_reg + ", 0");
        emit("b LBL" + std::to_string(label_count + 1));
        emit("LBL" + std::to_string(label_count) + ":");
        emit("li " + result_bool_reg + ", 1");
        emit("LBL" + std::to_string(label_count + 1) + ":");
        label_count += 2;

        if (comp_op_str == "~=") {
            emit("xori " + result_bool_reg + ", " + result_bool_reg + ", 1");
        }

        free_expr_reg(TypeEnum::Float);
        free_expr_reg(TypeEnum::Float);
    }

    temp_bool_var_label = new_temporary_variable_label(TypeEnum::Integer);
    emit("sw " + result_bool_reg + ", " + temp_bool_var_label);
    free_expr_reg(TypeEnum::Integer);

    ExpressionNode* result_node = new ExpressionNode();
    result_node->type = TypeEnum::Integer;
    result_node->is_literal = false;
    result_node->name = temp_bool_var_label;

    delete op1;
    delete op2;
    free(comp_op_chars);
    return result_node;
}

ExpressionNode* przyp(const std::string& target_name, ExpressionNode* val_expr) {
    if (sym_table.find(target_name) == sym_table.end()) {
        if(val_expr) delete val_expr;
        throwCompilerError(CompilerError::VariableNotDeclared, "assignment target: " + target_name);
    }
    if (!val_expr) {
        throwCompilerError(CompilerError::NullExpressionInternal, "Assignment RHS expression is null");
    }
    TypeEnum target_type = sym_table[target_name].type;
    std::string val_reg_str;
    try {
        if (target_type == val_expr->type) {
            val_reg_str = load_expr_to_reg(val_expr);
            if (target_type == TypeEnum::Integer) emit("sw " + val_reg_str + ", " + target_name);
            else emit("s.s " + val_reg_str + ", " + target_name);
            free_expr_reg(target_type);
        } else if (target_type == TypeEnum::Float && val_expr->type == TypeEnum::Integer) {
            // Convert int to float
            val_reg_str = load_expr_to_reg(val_expr);
            std::string float_reg = new_temp_float_register();
            emit("mtc1 " + val_reg_str + ", " + float_reg);
            emit("cvt.s.w " + float_reg + ", " + float_reg);
            emit("s.s " + float_reg + ", " + target_name);
            free_expr_reg(TypeEnum::Integer);
            free_temp_float_register();
        } else if (target_type == TypeEnum::Integer && val_expr->type == TypeEnum::Float) {
            // Convert float to int
            val_reg_str = load_expr_to_reg(val_expr);
            std::string int_reg = new_temp_int_register();
            emit("cvt.w.s " + val_reg_str + ", " + val_reg_str);
            emit("mfc1 " + int_reg + ", " + val_reg_str);
            emit("sw " + int_reg + ", " + target_name);
            free_expr_reg(TypeEnum::Float);
            free_temp_int_register();
        } else {
            throwCompilerError(CompilerError::TypeMismatchAssign, 
                "Cannot assign " + type_to_string(val_expr->type) + " to " + type_to_string(target_type));
        }
    } catch (...) {
        delete val_expr;
        throw;
    }
    delete val_expr;
    return nullptr;  // Assignment doesn't return a value
}
ExpressionNode* process_binary_op(ExpressionNode* op1, ExpressionNode* op2,
    const std::string& op_str) {
    if (!op1 || !op2) {
        if (op1) delete op1;
        if (op2) delete op2;
        throwCompilerError(CompilerError::NullOperandInternal, "Binary operation");
    }

    TypeEnum type1 = op1->type;
    TypeEnum type2 = op2->type;
    TypeEnum result_type;

    // Determine result type
    if (type1 == TypeEnum::Float || type2 == TypeEnum::Float) {
        result_type = TypeEnum::Float;
    } else {
        result_type = TypeEnum::Integer;
    }

    // Handle type promotion
    handle_type_promotion(op1, op2, type1, type2);

    if (type1 != type2) {
        throwCompilerError(CompilerError::InternalLogicError, "Type mismatch after promotion");
    }

    std::string result_reg;
    std::string temp_result_label;

    if (type1 == TypeEnum::Integer) {
        // Integer operations
        std::string r1_str = load_expr_to_reg(op1);
        std::string r2_str = load_expr_to_reg(op2);
        result_reg = new_temp_int_register();

        if (op_str == "+") { emit("add " + result_reg + ", " + r1_str + ", " + r2_str); }
        else if (op_str == "-") { emit("sub " + result_reg + ", " + r1_str + ", " + r2_str); }
        else if (op_str == "*") { emit("mul " + result_reg + ", " + r1_str + ", " + r2_str); }
        else if (op_str == "/") {
            emit("div " + r1_str + ", " + r2_str);
            emit("mflo " + result_reg);
        }

        free_expr_reg(TypeEnum::Integer);
        free_expr_reg(TypeEnum::Integer);
    } else {
        // Float operations
        std::string fr1_str = load_expr_to_reg(op1);
        std::string fr2_str = load_expr_to_reg(op2);
        result_reg = new_temp_float_register();

        if (op_str == "+") { emit("add.d " + result_reg + ", " + fr1_str + ", " + fr2_str); }
        else if (op_str == "-") { emit("sub.d " + result_reg + ", " + fr1_str + ", " + fr2_str); }
        else if (op_str == "*") { emit("mul.d " + result_reg + ", " + fr1_str + ", " + fr2_str); }
        else if (op_str == "/") {
            emit("div.s " + result_reg + ", " + fr1_str + ", " + fr2_str);
        }

        free_expr_reg(TypeEnum::Float);
        free_expr_reg(TypeEnum::Float);
    }

    temp_result_label = new_temporary_variable_label(result_type);
    if (result_type == TypeEnum::Integer) {
        emit("sw " + result_reg + ", " + temp_result_label);
    } else {
        emit("s.s " + result_reg + ", " + temp_result_label);
    }
    free_expr_reg(result_type);

    ExpressionNode* result = new ExpressionNode();
    result->type = result_type;
    result->is_literal = false;
    result->name = temp_result_label;

    delete op1;
    delete op2;
    return result;
} 