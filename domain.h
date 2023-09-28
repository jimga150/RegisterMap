#ifndef DOMAIN_H
#define DOMAIN_H

#include <string>

class Domain
{
public:
    Domain();

    std::string interface;

    uint64_t low_addr;
    uint64_t high_addr;


    uint64_t get_size(){
        return this->high_addr - this->low_addr;
    }
};

#endif // DOMAIN_H
