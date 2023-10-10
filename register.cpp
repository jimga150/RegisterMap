#include "register.h"

Register::Register()
{

}

Register::~Register()
{

}

uint32_t Register::getByteLen(){
    float frac_byte_cnt = ((float)bit_len)/8.0;
    float whole_byte_cnt = ceil(frac_byte_cnt);
    return (uint32_t)whole_byte_cnt;
}

//bitstring Register::getDefaultVal(){
//    bitstring ans;
//    for (uint32_t i = 0; i < bit_len; ++i){
//        ans.push_back(0);
//    }
//    for (BitField* bf : this->bitfields){
//        for (uint32_t i = bf->low_index; i <= bf->high_index; ++i){
//            ans.assign(i, bf->default_val.at(i-bf->low_index));
//        }
//    }
//    return ans;
//}
