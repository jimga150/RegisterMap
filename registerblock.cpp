#include "registerblock.h"

RegisterBlock::RegisterBlock()
{
    invalid_reg.name = RegisterBlock::invalid_reg_name;
}

Register* RegisterBlock::get_register_at(addr_t offset){
    for (Register& reg : this->registers){
        if (reg.offset == offset) return &reg;
    }
    return &invalid_reg;
}

addr_t RegisterBlock::get_max_offset()
{
    addr_t max_offset = 0;
    for (Register& r : this->registers){
        if (r.offset > max_offset) max_offset = r.offset;
    }
    return max_offset;
}
