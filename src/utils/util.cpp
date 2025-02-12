#include "util.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

void exitOnCriticalError(const std::string &errorMessage, const std::string &errorPlace) {
    std::cerr << "Error: " << errorMessage << std::endl;
    std::cerr << "Error from function : " << errorPlace << std::endl;
}

std::string file2String(const std::string &filename) {
    std::ifstream input(filename.c_str());
    if(!input){
      exitOnCriticalError("[Shader Program][file2String] Error: cannot open " + filename, "file2String");
    }
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

char* file2CharArray(const std::string &filename) {
    std::ifstream input(filename.c_str());
    if(!input)
      exitOnCriticalError("[Shader Program][file2String] Error: cannot open " + filename, "file2String");
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string str = buffer.str();
    char* cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}