#ifndef BITFIELD_H
#define BITFIELD_H

#include <map>
#include <vector>

#include "common.h"

#define FOREACH_IOPERMS(FXN) \
    FXN(NA) \
    FXN(RO) \
    FXN(WO) \
    FXN(RW) \
    FXN(num_ioperms)

typedef uint8_t bit;

typedef std::vector<bit> bitstring;

enum ioperm_enum {
    FOREACH_IOPERMS(GENERATE_ENUM)
};

static const char* ioperm_str[] {
    FOREACH_IOPERMS(GENERATE_STRING)
};

class BitField
{
public:
    BitField();

    uint32_t high_index;
    uint32_t low_index;

    inline static const std::string invalid_bitfield_name = "<INVALID>";

    std::string name;
    std::string codename;

//    bool readable = true;
//    bool writeable; //TODO: add write condition functionality

    bitstring default_val;

    //TODO: add reset domains

    std::string description = "";

    bool is_enum = false;
    std::map<bitstring, std::string> values;

    bool self_clearing = false;
    bitstring cleared_state;


//    ioperm_enum get_ioperms(){
//        if (this->readable){
//            if (this->writeable){
//                return RW;
//            } else {
//                return RO;
//            }
//        } else {
//            if (this->writeable){
//                return WO;
//            } else {
//                return NA;
//            }
//        }
//        return num_ioperms;
//    }
//    std::string get_ioperms_string(){
//        return ioperm_str[this->get_ioperms()];
//    }
};

#endif // BITFIELD_H
