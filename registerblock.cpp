#include "registerblock.h"

RegisterBlock::RegisterBlock()
{
    invalid_reg.name = RegisterBlock::invalid_reg_name;
}

RegisterBlock::~RegisterBlock()
{

}

//Register* RegisterBlock::getRegisterAt(addr_t offset){
//    for (Register* reg : this->registers){
//        if (reg->offset == offset) return reg;
//    }
//    return &invalid_reg;
//}

//addr_t RegisterBlock::getMaxOffset()
//{
//    addr_t max_offset = 0;
//    for (Register* r : this->registers){
//        if (r->offset > max_offset) max_offset = r->offset;
//    }
//    return max_offset;
//}
