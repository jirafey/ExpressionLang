CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Source files
SRCS = code_gen_globals.cc code_gen_mips_output.cc code_gen_binary_ops.cc code_gen_registers.cc code_gen_labels.cc code_gen_string_utils.cc lex.yy.c bison.tab.cc
OBJS = $(SRCS:.cc=.o)

# Test directories
TEST_DIR = tests
OUTPUT_DIR = $(TEST_DIR)/output
OUTPUT_ASM = $(OUTPUT_DIR)/asm
OUTPUT_ERR = $(OUTPUT_DIR)/err
OUTPUT_OUT = $(OUTPUT_DIR)/out

# Test categories (excluding read tests)
ARITHMETIC_TESTS = $(TEST_DIR)/arithmetic/float_arithmetic.xprshn \
                   $(TEST_DIR)/arithmetic/int_arithmetic.xprshn \
                   $(TEST_DIR)/arithmetic/mixed_arithmetic.xprshn

GOOD_TESTS = $(TEST_DIR)/good/negatives.xprshn \
             $(TEST_DIR)/good/if_simple.xprshn \
             $(TEST_DIR)/good/if_float_compare.xprshn \
             $(TEST_DIR)/good/if_mixed_compare.xprshn \
             $(TEST_DIR)/good/if_int_compare.xprshn \
             $(TEST_DIR)/good/basic_arithmetics_float.xprshn \
             $(TEST_DIR)/good/print_float.xprshn \
             $(TEST_DIR)/good/literal_ops.xprshn \
             $(TEST_DIR)/good/conversion_ops.xprshn \
             $(TEST_DIR)/good/conversion_assign.xprshn \
             $(TEST_DIR)/good/comments_and_structure.xprshn \
             $(TEST_DIR)/good/basic_arithmetics_int.xprshn \
             $(TEST_DIR)/good/empty.xprshn \
             $(TEST_DIR)/good/print_int.xprshn \
             $(TEST_DIR)/good/float_ops.xprshn \
             $(TEST_DIR)/good/int_ops.xprshn

COMPILE_ERROR_TESTS = $(TEST_DIR)/compile_error/extra_paren.xprshn \
                     $(TEST_DIR)/compile_error/missing_excl.xprshn \
                     $(TEST_DIR)/compile_error/assign_undeclared.xprshn \
                     $(TEST_DIR)/compile_error/standalone_expr.xprshn \
                     $(TEST_DIR)/compile_error/if_non_bool.xprshn \
                     $(TEST_DIR)/compile_error/nested_expr.xprshn \
                     $(TEST_DIR)/compile_error/chained_assignment.xprshn \
                     $(TEST_DIR)/compile_error/invalid_op.xprshn \
                     $(TEST_DIR)/compile_error/redeclare_var.xprshn \
                     $(TEST_DIR)/compile_error/undeclared_var.xprshn

CONVERSION_TESTS = $(TEST_DIR)/conversion/int_to_float.xprshn \
                  $(TEST_DIR)/conversion/float_to_int.xprshn

# Main targets
Expression: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Create output directories
$(shell mkdir -p $(OUTPUT_ASM) $(OUTPUT_ERR) $(OUTPUT_OUT))

# Test targets
test: test_arithmetic test_good test_compile_error test_conversion

test_arithmetic:
	@echo "Running arithmetic tests..."
	@for test in $(ARITHMETIC_TESTS); do \
		basename=$$(basename $$test .xprshn); \
		./Expression < $$test > $(OUTPUT_ASM)/$$basename.asm 2> $(OUTPUT_ERR)/$$basename.err; \
		if [ -s $(OUTPUT_ERR)/$$basename.err ]; then \
			echo "Error in $$basename"; \
		fi \
	done

test_good:
	@echo "Running good tests..."
	@for test in $(GOOD_TESTS); do \
		basename=$$(basename $$test .xprshn); \
		./Expression < $$test > $(OUTPUT_ASM)/$$basename.asm 2> $(OUTPUT_ERR)/$$basename.err; \
		if [ -s $(OUTPUT_ERR)/$$basename.err ]; then \
			echo "Error in $$basename"; \
		fi \
	done

test_compile_error:
	@echo "Running compile error tests..."
	@for test in $(COMPILE_ERROR_TESTS); do \
		basename=$$(basename $$test .xprshn); \
		./Expression < $$test > $(OUTPUT_ASM)/$$basename.asm 2> $(OUTPUT_ERR)/$$basename.err; \
		if [ ! -s $(OUTPUT_ERR)/$$basename.err ]; then \
			echo "Expected error but none found in $$basename"; \
		fi \
	done

test_runtime_error:
	@echo "Running runtime error tests..."
	@for test in $(RUNTIME_ERROR_TESTS); do \
		basename=$$(basename $$test .xprshn); \
		./Expression < $$test > $(OUTPUT_ASM)/$$basename.asm 2> $(OUTPUT_ERR)/$$basename.err; \
		if ! grep -q "_runtime_error_handler" $(OUTPUT_ASM)/$$basename.asm; then \
			echo "Expected runtime error handler but none found in $$basename"; \
		fi \
	done

test_conversion:
	@echo "Running conversion tests..."
	@for test in $(CONVERSION_TESTS); do \
		basename=$$(basename $$test .xprshn); \
		./Expression < $$test > $(OUTPUT_ASM)/$$basename.asm 2> $(OUTPUT_ERR)/$$basename.err; \
		if [ -s $(OUTPUT_ERR)/$$basename.err ]; then \
			echo "Error in $$basename"; \
		fi \
	done

# Build rules
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

lex.yy.c: flex.l
	flex $<

bison.tab.cc bison.tab.hh: bison.yy
	bison -d $<

# Cleanup
clean:
	rm -f Expression *.o lex.yy.c bison.tab.*
	rm -rf $(OUTPUT_DIR)/*

.PHONY: test test_arithmetic test_good test_compile_error test_runtime_error test_conversion clean