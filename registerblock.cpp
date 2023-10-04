#include "registerblock.h"

RegisterBlock::RegisterBlock()
{

}

Register RegisterBlock::get_register_at(addr_t offset){
    for (Register& reg : this->registers){
        if (reg.offset == offset) return reg;
    }
    Register r;
    r.name = invalid_reg_name;
    return r;
}

addr_t RegisterBlock::get_max_offset()
{
    addr_t max_offset = 0;
    for (Register& r : this->registers){
        if (r.offset > max_offset) max_offset = r.offset;
    }
    return max_offset;
}

void RegisterBlock::sort_registers_by_offset(){
    std::vector<Register> sorted_registers;
    addr_t max_offset = this->get_max_offset();

    for (addr_t i = 0; i <= max_offset; ++i){
        Register r = this->get_register_at(i);
        if (!(r.name.compare(invalid_reg_name))) continue;
        sorted_registers.push_back(r);
    }

    this->registers = sorted_registers;
}
