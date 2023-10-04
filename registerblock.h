#ifndef REGISTERBLOCK_H
#define REGISTERBLOCK_H

#include "register.h"
#include "domain.h"

//TODO: make register groups that can own ranges of registers within a block

class RegisterBlock
{
public:
    RegisterBlock();

    std::vector<Register> registers;

    std::string name;
    std::string code_name;

    addr_t size;

//    std::map<uint64_t, Domain*> domain_offsets;

    inline static const std::string invalid_reg_name = "<INVALID>";

    Register get_register_at(addr_t offset);

    addr_t get_max_offset();

    void sort_registers_by_offset();

};

#endif // REGISTERBLOCK_H
