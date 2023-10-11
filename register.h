#ifndef REGISTER_H
#define REGISTER_H

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdio>

typedef uint32_t addr_t;

class Register
{
public:
    Register();
    ~Register();

    std::string name;
    std::string code_name;

    addr_t offset;

    std::string description;

    //TODO: derive reset domains with reset domains of bit fields
    //or maybe assume that resets apply to entire registers?
};

#endif // REGISTER_H
