#ifndef COMMON_H
#define COMMON_H

#include <string>

//stringifies name of variable or literal you give it
//str(TEMP) --> "TEMP"
#define STR(x) #x

//stringifies value of variable or literal you give it
//xstr(TEMP) --> "10.3"
//(assuming TEMP == 10.3)
#define XSTR(x) STR(x)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define BITS_PER_BYTE (8)

#define FOREACH_MEM_INTFACE(FXN) \
FXN(AXI4) \
FXN(AXI4LITE) \
FXN(NATIVE) \
FXN(AVALON) \
FXN(APB) \
FXN(PRIM)

enum mem_intface_t {
    FOREACH_MEM_INTFACE(GENERATE_ENUM)
};

static const char* mem_intface_str[]{
    FOREACH_MEM_INTFACE(GENERATE_STRING)
};

typedef uint32_t addr_t;

//replaces the first instance of toReplace within s with the string replaceWith
//returns
//  true if replacement was succesful
//  false if replacement was unsuccessful (nothing happened)
bool replace_first(std::string& s, const std::string& toReplace, const std::string& replaceWith);

std::string generate_code_name(std::string name);

#endif // COMMON_H
