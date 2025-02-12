#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

void exitOnCriticalError(const std::string &errorMessage, const std::string &errorPlace);
std::string file2String(const std::string &filename);
char* file2CharArray(const std::string &filename);

#endif // UTIL_H