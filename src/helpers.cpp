#include "../incl/Command.hpp"

std::vector<std::string> splitVec(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::set<std::string> splitSet(const std::string& input, char delimiter) {
    std::set<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.insert(token);
    }
    return tokens;
}

const std::string joinVecIntoStr(std::vector<std::string>::const_iterator start,
                                std::vector<std::string>::const_iterator end)
{
    std::string result = "";
    for (std::vector<std::string>::const_iterator it = start; it != end; it++)
        result += (*it + " ");
    return result;
}

const std::string joinSetIntoStr(std::set<std::string>::const_iterator start,
                                std::set<std::string>::const_iterator end)
{
    std::string result = "";
    for (std::set<std::string>::const_iterator it = start; it != end; it++)
        result += (*it + " ");
    return result;
}