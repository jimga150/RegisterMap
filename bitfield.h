#ifndef BITFIELD_H
#define BITFIELD_H

#include <map>
#include <string>

//stringifies name of variable or literal you give it
//str(TEMP) --> "TEMP"
#define str(x) #x

//stringifies value of variable or literal you give it
//xstr(TEMP) --> "10.3"
//(assuming TEMP == 10.3)
#define xstr(x) str(x)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_IOPERMS(FXN) \
    FXN(NA) \
    FXN(RO) \
    FXN(WO) \
    FXN(RW) \
    FXN(num_ioperms)

//TODO: come up with a type to use for representing arbitrarily long sets of bits

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

    std::string name;

    bool readable;
    bool writeable; //TODO: add write condition functionality

    //TODO: use a type that is bit-width scaleable, like bigint, for anything storing a value this bitfield could have
    uint32_t default_val;

    //TODO: add reset domains

    std::string description;

    bool is_enum;
    std::map<std::string, uint32_t> values;

    bool self_clearing;
    uint32_t cleared_state;


    ioperm_enum get_ioperms(){
        if (this->readable){
            if (this->writeable){
                return RW;
            } else {
                return RO;
            }
        } else {
            if (this->writeable){
                return WO;
            } else {
                return NA;
            }
        }
        return num_ioperms;
    }
    std::string get_ioperms_string(){
        return ioperm_str[this->get_ioperms()];
    }
};

#endif // BITFIELD_H
