#ifndef REGISTER_H
#define REGISTER_H

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdio>

#include "bitfield.h"

typedef uint32_t addr_t;

class Register
{
public:
    Register();
    ~Register();

    std::string name;
    std::string code_name;

    uint32_t bit_len;

    addr_t offset;

    std::string description;

    std::vector<BitField*> bitfields;

    uint32_t get_byte_len();

    bitstring get_default_val();

    //TODO: derive reset domains with reset domains of bit fields
    //or maybe assume that resets apply to entire registers?
};

#endif // REGISTER_H
