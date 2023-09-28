#ifndef REGISTER_H
#define REGISTER_H

#include <stdexcept>
#include <string>
#include <vector>

#include "bitfield.h"

//replaces the first instance of toReplace within s with the string replaceWith
//returns
//  true if replacement was succesful
//  false if replacement was unsuccessful
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

    void generate_code_name(){

    }
};

#endif // REGISTER_H
