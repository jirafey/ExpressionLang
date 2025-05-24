%code requires {
    #include "header/expression.h"
    #include "header/compiler_error.h"
    #include "header/code_gen_globals.h"
    #include "header/code_gen_helpers.h"
}

%{
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <map>

#include "header/compiler_error.h"
#include "header/code_gen_globals.h"
#include "header/code_gen_helpers.h"
#include "header/expression.h"

int yylex();
void yyerror(const char* msg);
extern int yylineno;

// Global error flag
bool has_compiler_error = false;

// Helper function to safely duplicate strings
char* safe_strdup(const char* s, const char* context = "") {
    if (!s) {
        throwCompilerError(CompilerError::NullPointerInternal, 
            std::string("safe_strdup called with null string in context: ") + context);
    }
    char* result = strdup(s);
    if (!result) {
        throwCompilerError(CompilerError::AllocationFailed, 
            std::string("strdup failed in context: ") + context);
    }
    return result;
}
%}

%union {
    char* name;
    ExpressionNode* expr;
    TypeEnum enum_type;
    int ival;
    float fval;
}

%start program
%token <name> ID
%token <ival> INT_LITERAL
%token <fval> FLOAT_LITERAL
%token INT_TYPE FLOAT_TYPE EXCL ASSIGN PLUS MINUS MUL DIV
%token LPAREN RPAREN PRINT READ IF
%token EQ_OP NE_OP LT_OP LE_OP GT_OP GE_OP
%type <expr> expr
%type <enum_type> type_specifier
%type <name> comp_operator
%type <expr> przypisanie
%type <expr> read_stmt
%type <expr> print_stmt
%left PLUS MINUS
%left MUL DIV
%type <expr> cond_expr

%%

/* --- Grammar Rules --- */

program: /* empty program is allowed */
    | top_level_item_list
        {
            if (!has_compiler_error) {
                // Only output MIPS if no errors occurred
                output_mips_code();
            } else {
                // If there were errors, output a minimal MIPS program that just exits
                std::cout << ".text\n.globl main\nmain:\n\tli $v0, 10\n\tsyscall\n";
            }
        }
    ;

top_level_item_list:
      top_level_item
    | top_level_item_list top_level_item
    ;

top_level_item:
      declaration
    | statement
    ;

declaration: type_specifier ID EXCL
    {
        Symbol sym;
        sym.type = $1;
        std::string var_name = $2;
        if (sym_table.count(var_name)) {
            free($2);
            throwCompilerError(CompilerError::SemanticError, "Multiple declaration of " + var_name);
        }
        sym.name = var_name;
        sym_table[var_name] = sym;

        if ($1 == TypeEnum::Integer) {
            emit_data(var_name + ": .word 0");
        } else if ($1 == TypeEnum::Float) {
            emit_data(var_name + ": .float 0.0");
        } else {
            free($2);
            throwCompilerError(CompilerError::UnsupportedTypeInternal,
                "Declaration for unknown type code " + std::to_string(static_cast<int>($1)));
        }
        free($2);
    }
    ;

type_specifier:
      INT_TYPE      { $$ = TypeEnum::Integer; }
    | FLOAT_TYPE    { $$ = TypeEnum::Float; }
    ;

statement:
      przypisanie EXCL
    | expr EXCL
        {
            if($1) {
                // Check if this is a standalone expression (not an assignment)
                if($1->is_literal || !$1->name.empty()) {
                    throwCompilerError(CompilerError::SemanticError, "Standalone expressions are not allowed");
                }
                delete $1;
            } else {
                throwCompilerError(CompilerError::NullExpressionInternal, "Standalone expression statement (expr EXCL) evaluated to null");
            }
        }
    | print_stmt EXCL
    | read_stmt EXCL
    | if_expression
    ;

read_stmt: READ ID
        {
            std::string var_name = $2;
            $$ = read(var_name);
            free($2);
        }
    ;

print_stmt: PRINT expr
    {
        ExpressionNode* expr_to_print = $2;
        $$ = print(expr_to_print);
        delete expr_to_print;
    }
    ;

przypisanie: ID ASSIGN expr
    {
        std::string target_name = $1;
        ExpressionNode* val_expr = $3;
        $$ = przyp(target_name, val_expr);
        free($1);
    }
    ;

expr: INT_LITERAL { $$ = new ExpressionNode(TypeEnum::Integer, $1); }
    | FLOAT_LITERAL { $$ = new ExpressionNode(TypeEnum::Float, $1); }
    | ID { 
        std::string var_name = $1;
        if (sym_table.find(var_name) == sym_table.end()) {
            free($1);
            throwCompilerError(CompilerError::VariableNotDeclared, var_name);
        }
        $$ = new ExpressionNode(sym_table[var_name].type, var_name);
        free($1);
    }
    | LPAREN expr RPAREN { $$ = $2; }
    | expr PLUS expr { 
        ExpressionNode* op1 = $1;
        ExpressionNode* op2 = $3;
        if (!op1 || !op2) {
            if(op1) delete op1;
            if(op2) delete op2;
            throwCompilerError(CompilerError::NullOperandInternal, "Addition operation");
        }
        $$ = process_binary_op(op1, op2, "+");
    }
    | expr MINUS expr { 
        ExpressionNode* op1 = $1;
        ExpressionNode* op2 = $3;
        if (!op1 || !op2) {
            if(op1) delete op1;
            if(op2) delete op2;
            throwCompilerError(CompilerError::NullOperandInternal, "Subtraction operation");
        }
        $$ = process_binary_op(op1, op2, "-");
    }
    | expr MUL expr { 
        ExpressionNode* op1 = $1;
        ExpressionNode* op2 = $3;
        if (!op1 || !op2) {
            if(op1) delete op1;
            if(op2) delete op2;
            throwCompilerError(CompilerError::NullOperandInternal, "Multiplication operation");
        }
        $$ = process_binary_op(op1, op2, "*");
    }
    | expr DIV expr { 
        ExpressionNode* op1 = $1;
        ExpressionNode* op2 = $3;
        if (!op1 || !op2) {
            if(op1) delete op1;
            if(op2) delete op2;
            throwCompilerError(CompilerError::NullOperandInternal, "Division operation");
        }
        $$ = process_binary_op(op1, op2, "/");
    }
    ;

// --- IF Statement ---
if_expression:
    IF LPAREN cond_expr RPAREN statement
        {
            ExpressionNode* condition = $3;
            if (!condition) throwCompilerError(CompilerError::NullExpressionInternal, "IF condition expression is null");
            if (condition->type != TypeEnum::Integer) { 
                delete condition; 
                throwCompilerError(CompilerError::IfConditionNotBool, "IF condition must be an integer expression"); 
            }
            std::string cond_result_reg = load_expr_to_reg(condition);
            std::string end_if_label_str = new_control_label();
            emit("beqz " + cond_result_reg + ", " + end_if_label_str);
            free_expr_reg(TypeEnum::Integer);
            delete condition;
            emit(end_if_label_str + ":");
        }
    ;

// Condition expression (formatted in previous answer)
cond_expr: expr comp_operator expr
        {
            ExpressionNode* op1 = $1;
            char* comp_op_chars = $2;
            ExpressionNode* op2 = $3;
            ExpressionNode* result_node = nullptr;

            if (!op1 || !op2) {
                if(op1) delete op1;
                if(op2) delete op2;
                if(comp_op_chars) free(comp_op_chars);
                throwCompilerError(CompilerError::NullOperandInternal, "Comparison operation");
            }

            std::string comp_op_str(comp_op_chars);
            TypeEnum type1 = op1->type;
            TypeEnum type2 = op2->type;

            try {
                // Handle type promotion using the shared function
                handle_type_promotion(op1, op2, type1, type2);

                $$ = decl(op1, comp_op_chars, op2);
            } catch (...) {
                delete op1;
                delete op2;
                free(comp_op_chars);
                throw;
            }

            delete op1;
            delete op2;
            free(comp_op_chars);
            $$ = result_node;
        }
    ;

comp_operator: // Returns the operator string via safe_strdup
      EQ_OP   { $$ = safe_strdup("=="); }
    | NE_OP   { $$ = safe_strdup("~="); }
    | LT_OP   { $$ = safe_strdup("<"); }
    | LE_OP   { $$ = safe_strdup("<="); }
    | GT_OP   { $$ = safe_strdup(">"); }
    | GE_OP   { $$ = safe_strdup(">="); }
    ;

%%

int main() {
    try {
        yyparse();
        return has_compiler_error ? 1 : 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}

void yyerror(const char* msg) {
    std::cerr << "Error on line " << yylineno << ": " << msg << std::endl;
    has_compiler_error = true;
}