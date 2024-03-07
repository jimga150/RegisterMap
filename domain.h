#ifndef DOMAIN_H
#define DOMAIN_H

#include <string>

#include "common.h"

class Domain
{
public:
    Domain();

    std::string name;
    std::string codename;

    mem_intface_t protocol;

    uint8_t data_width_bytes;

    addr_t low_addr;
    addr_t high_addr;

    addr_t getSize(){
        return this->high_addr - this->low_addr;
    }
};

#endif // DOMAIN_H
