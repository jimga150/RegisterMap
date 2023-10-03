#include "common.h"

bool replace_first(std::string& s, const std::string& toReplace, const std::string& replaceWith){
    std::size_t pos = s.find(toReplace);
    if (pos == std::string::npos) return false;

    try{
        s.replace(pos, toReplace.length(), replaceWith);
    } catch (const std::out_of_range& e){
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s", __FILE__, __LINE__, e.what());
        return false;
    } catch (const std::length_error& e){
        fprintf(stderr, "%s:%d: Length error exception thrown! %s", __FILE__, __LINE__, e.what());
        return false;
    }

    return true;
}

std::string generate_code_name(std::string name){
    //copy name from human readable name
    std::string code_name = name;

    //    printf("generate_code_name called: %s\n", code_name.c_str());

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

    if (code_name.size() > 0 && code_name.at(0) <= '9' && code_name.at(0) >= '0'){
        //names in source code cannot start with numbers.
        //On the chance that whatever name this is is used as the very first part of a source name,
        //throw an underscore in front of any leading numbers
        code_name.insert(0, 1, '_');
    }

    return code_name;
}
