#include "header/code_gen_globals.h"
#include "header/code_gen_helpers.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// MIPS output functions
void output_mips_code() {
    // Output data section
    std::cout << ".data\n";
    for (const auto& data : data_segment) {
        std::cout << data << "\n";
    }
    std::cout << "_newline_char: .asciiz \"\\n\"\n";

    // Output text section
    std::cout << "\n.text\n";
    std::cout << ".globl main\n";
    std::cout << "main:\n";
    
    // Initialize $f0 to 0.0 for float comparisons
    std::cout << "\tmtc1 $zero, $f0\n";
    std::cout << "\tcvt.s.w $f0, $f0\n";
    
    // Output all regular code first
    for (const auto& text : text_segment) {
        std::cout << text << "\n";
    }
}

std::string new_float_literal(float value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << value;
    std::string label = new_temp_label();
    emit_data(label + ": .float " + ss.str());
    return label;
}