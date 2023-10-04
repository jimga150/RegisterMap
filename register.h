#ifndef REGISTER_H
#define REGISTER_H

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdio>

#include "bitfield.h"

typedef uint32_t addr_t;

class Register
{
public:
    Register();

    std::string name;
    std::string code_name;

    uint32_t bit_len;

    addr_t offset;

    std::string description;

    std::vector<BitField> bitfields;

    uint32_t get_byte_len(){
        float frac_byte_cnt = ((float)bit_len)/8.0;
        float whole_byte_cnt = ceil(frac_byte_cnt);
        return (uint32_t)whole_byte_cnt;
    }

    bitstring get_default_val(){
        bitstring ans;
        for (uint32_t i = 0; i < bit_len; ++i){
            ans.push_back(0);
        }
        for (BitField& bf : this->bitfields){
            for (uint32_t i = bf.low_index; i <= bf.high_index; ++i){
                ans.assign(i, bf.default_val.at(i-bf.low_index));
            }
        }
        return ans;
    }

    //TODO: derive reset domains with reset domains of bit fields
    //or maybe assume that resets apply to entire registers?
};

#endif // REGISTER_H
