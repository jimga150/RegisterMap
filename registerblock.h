#ifndef REGISTERBLOCK_H
#define REGISTERBLOCK_H

#include "register.h"
//#include "domain.h"

//TODO: make register groups that can own ranges of registers within a block

class RegisterBlock
{
public:
    RegisterBlock();
    ~RegisterBlock();

    uint32_t getByteLen();

//    Register* getRegisterAt(addr_t offset);

//    addr_t getMaxOffset();


    std::string name;
    std::string code_name;
    std::string description;

    addr_t size;

    uint32_t bit_len;

//    std::map<uint64_t, Domain*> domain_offsets;

    inline static const std::string invalid_reg_name = "<INVALID>";

    Register invalid_reg;

};

#endif // REGISTERBLOCK_H
