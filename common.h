#ifndef COMMON_H
#define COMMON_H

#include <string>

//stringifies name of variable or literal you give it
//str(TEMP) --> "TEMP"
#define STR(x) #x

//stringifies value of variable or literal you give it
//xstr(TEMP) --> "10.3"
//(assuming TEMP == 10.3)
#define XSTR(x) STR(x)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

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
        fprintf(stderr, "%s:%d: Out of range exception thrown! %s > %s.size()", __FILE__, __LINE__, STR(pos), STR(s));
        return false;
    } catch (std::length_error){
        fprintf(stderr, "%s:%d: Length error exception thrown! resulting string will be too large for buffer in %s", __FILE__, __LINE__, STR(s));
        return false;
    }

    return true;
}

static std::string generate_code_name(std::string name){
    //copy name from human readable name
    std::string code_name = name;

    printf("generate_code_name called: %s\n", code_name.c_str());

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

#endif // COMMON_H
