//
// Created by Ted Klein Bergman on 11/4/18.
//

#ifndef NEXT_UTILITIES_H
#define NEXT_UTILITIES_H


#include <vector>
#include <string>
#include <fstream>
#include <sstream>


std::string read(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Couldn't open file " + path + ".");

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}



std::vector<std::string> split(const std::string& source, const char delimiter)
{
    std::vector<std::string> result;

    unsigned long start = 0U;
    unsigned long end   = source.find(delimiter);
    
    while (end != std::string::npos)
    {
        result.emplace_back(source.substr(start, end - start));
        start = end + 1;
        end   = source.find(delimiter, start);
    }
    
    result.emplace_back(source.substr(start, end));
    return result;
}













#endif //NEXT_UTILITIES_H
