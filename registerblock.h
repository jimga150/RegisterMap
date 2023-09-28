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

    uint32_t size;

    std::map<uint64_t, Domain*> domain_offsets;


    Register get_register_at(uint32_t offset){
        for (Register& reg : this->registers){
            if (reg.offset == offset) return reg;
        }
        Register r;
        r.name = "<INVALID>";
        return r;
    }


};

#endif // REGISTERBLOCK_H
