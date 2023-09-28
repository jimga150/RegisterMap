#ifndef REGISTER_H
#define REGISTER_H

#include <stdexcept>
#include <string>
#include <vector>

#include "bitfield.h"

//replaces the first instance of toReplace within s with the string replaceWith
//returns
//  true if replacement was succesful
//  false if replacement was unsuccessful (nothing happened)
static bool replace_first(std::string& s, const std::string& toReplace, const std::string& replaceWith){
    std::size_t pos = s.find(toReplace);
    if (pos == std::string::npos) return false;

    try{
        s.replace(pos, toReplace.length(), replaceWith);
    } catch (std::out_of_range){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s > %s.size()", __FILE__, __LINE__, str(pos), str(s));
        return false;
    } catch (std::length_error){
        fprintf(stderr, "%s:%d: Length error exception thrown! resulting string will be too large for buffer in %s", __FILE__, __LINE__, str(s));
        return false;
    }

    return true;
}

static std::string generate_code_name(std::string name){
    //copy name from human readable name
    std::string code_name = name;

    //replace all spaces with underscores
    char toReplace = ' ';
    while(replace_first(code_name, std::string(1, toReplace), "_"));

    //remove anything thats not an underscore, number, or letter
    ++toReplace;
    for (; toReplace < '0'; ++toReplace){
        while(replace_first(code_name, std::string(1, toReplace), ""));
    }
    for (toReplace = '9' + 1; toReplace < 'A'; ++toReplace){
        while(replace_first(code_name, std::string(1, toReplace), ""));
    }
    for (toReplace = 'Z' + 1; toReplace < 'a'; ++toReplace){
        if (toReplace == '_') continue;
        while(replace_first(code_name, std::string(1, toReplace), ""));
    }
    for (toReplace = 'z' + 1; toReplace < 0x7F; ++toReplace){
        while(replace_first(code_name, std::string(1, toReplace), ""));
    }
    return code_name;
}

class Register
{
public:
    Register();

    std::string name;
    std::string code_name;

    uint32_t bit_len;

    uint32_t offset;

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
