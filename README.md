# ExpressionLang
Expression programming language for MIPS assembly language, Compilers project

# Example code:
```java
// Example ExpressionLang code:
int a!
float b!
a = 10!
b = a + 5.5!
print b!
if (a > 5)
    print a!
```

# Supported:
- `float` of single precision
- `int`
- basic arithmetic operations: `+`, `-`, `*`, `/`
- basic if statements (without else)
- conditional statements - `==`, `~=`, `>`,`>=`, `<`, `<=`

```md
.
├── bison.yy
├── code_gen_binary_ops.cc
├── code_gen_globals.cc
├── code_gen_labels.cc
├── code_gen_mips_output.cc
├── code_gen_registers.cc
├── code_gen_string_utils.cc
├── flex.l
├── header
│   ├── code_gen_globals.h
│   ├── code_gen_helpers.h
│   ├── code_gen_labels.h
│   ├── code_gen_mips_output.h
│   ├── compiler_error.h
│   ├── expression.h
│   └── TypeEnum.h
├── Makefile
├── README.md
└── tests
    ├── arithmetic
    │   ├── float_arithmetic.xprshn
    │   ├── int_arithmetic.xprshn
    │   └── mixed_arithmetic.xprshn
    ├── compile_error
    │   ├── assign_undeclared.xprshn
    │   ├── assign_uninitialized.xprshn
    │   ├── chained_assignment.xprshn
    │   ├── extra_paren.xprshn
    │   ├── if_non_bool.xprshn
    │   ├── invalid_op.xprshn
    │   ├── missing_excl.xprshn
    │   ├── nested_expr.xprshn
    │   ├── read_print_edge.xprshn
    │   ├── redeclare_var.xprshn
    │   ├── standalone_expr.xprshn
    │   ├── type_mismatch.xprshn
    │   ├── undeclared_var.xprshn
    │   └── uninitialized_var.xprshn
    ├── conversion
    │   ├── float_to_int.xprshn
    │   └── int_to_float.xprshn
    ├── good
    │   ├── basic_arithmetics_float.xprshn
    │   ├── basic_arithmetics_int.xprshn
    │   ├── comments_and_structure.xprshn
    │   ├── conversion_assign.xprshn
    │   ├── conversion_ops.xprshn
    │   ├── empty.xprshn
    │   ├── float_ops.xprshn
    │   ├── if_float_compare.xprshn
    │   ├── if_int_compare.xprshn
    │   ├── if_mixed_compare.xprshn
    │   ├── if_simple.xprshn
    │   ├── int_ops.xprshn
    │   ├── literal_ops.xprshn
    │   ├── negatives.xprshn
    │   ├── print_float.xprshn
    │   ├── print_int.xprshn
    │   ├── read_float.xprshn
    │   └── read_int.xprshn
    └── output
```
# How to use:

```bash
make
```

```bash
make test
```

output of tests can be found in `tests/output`

to try the compiled MIPS code, download and use `Mars4_5.jar`

Running Mars:
```bash
# cd ~/Downloads
java -jar Mars4_5.jar
```
most likely the path will be ~/Downloads/Mars4_5.jar

