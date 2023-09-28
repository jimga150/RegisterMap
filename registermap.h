#ifndef REGISTERMAP_H
#define REGISTERMAP_H

#include <vector>

#include "domain.h"
#include "registerblock.h"

class RegisterMap
{
public:
    RegisterMap();

    std::vector<Domain> domains;
    std::vector<RegisterBlock> reg_blocks;


    RegisterBlock* add_register_block();
};

#endif // REGISTERMAP_H
