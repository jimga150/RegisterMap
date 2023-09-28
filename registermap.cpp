#include "registermap.h"

RegisterMap::RegisterMap()
{

}

RegisterBlock *RegisterMap::add_register_block(){
    this->reg_blocks.push_back(RegisterBlock());
    return &(this->reg_blocks.at(this->reg_blocks.size()-1));
}
