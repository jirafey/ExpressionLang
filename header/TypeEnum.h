#ifndef TYPEENUM_H
#define TYPEENUM_H

#include <string>

enum class TypeEnum {
    Integer,
    Float
};

inline std::string type_to_string(TypeEnum type) {
    switch (type) {
        case TypeEnum::Integer:
            return "int";
        case TypeEnum::Float:
            return "float";
        default:
            return "unknown";
    }
}

#endif // TYPEENUM_H 