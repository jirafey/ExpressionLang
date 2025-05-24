#include "header/code_gen_helpers.h"
#include "header/code_gen_globals.h"
#include "header/compiler_error.h"
#include "header/expression.h"

#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>

// --- String Manipulation ---
char* safe_strdup(const char* s, const std::string& context) {
    if (!s) {
        throwCompilerError(CompilerError::InternalLogicError, "safe_strdup null: " + context);
    }
    errno = 0;
    char* new_str = strdup(s);
    if (!new_str) {
        std::string err_ctx = "strdup failed in " + context;
        if (errno == ENOMEM) {
            err_ctx += ": Out of memory";
        } else if (errno != 0) {
            err_ctx += ": " + std::string(strerror(errno));
        }
        throwCompilerError(CompilerError::AllocationFailed, err_ctx);
    }
    return new_str;
} 